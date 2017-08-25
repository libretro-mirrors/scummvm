#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "base/main.h"
#include "common/scummsys.h"
#include "graphics/surface.libretro.h"
#include "audio/mixer_intern.h"

#include "os.h"
#include <libco.h>
#include "libretro.h"

#include <unistd.h>
/**
 * Include libgen.h for basename() and dirname().
 * @see http://linux.die.net/man/3/basename
 */
#include <libgen.h>
#include <string.h>

retro_log_printf_t log_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;
static retro_input_poll_t poll_cb = NULL;
static retro_input_state_t input_cb = NULL;

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_cb = cb; }

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;
   bool tmp = true;
   environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &tmp);
}

bool FRONTENDwantsExit;
bool EMULATORexited;

cothread_t mainThread;
cothread_t emuThread;

static char cmd_params[20][200];
static char cmd_params_num;

void retro_leave_thread(void)
{
   co_switch(mainThread);
}

static void retro_start_emulator(void)
{
   g_system = retroBuildOS();

   static const char* argv[20];
   for(int i=0; i<cmd_params_num; i++)
      argv[i] = cmd_params[i];

   scummvm_main(cmd_params_num, argv);
   EMULATORexited = true;

   if (log_cb)
      log_cb(RETRO_LOG_INFO, "Emulator loop has ended.\n");

   // NOTE: Deleting g_system here will crash...
}

static void retro_wrap_emulator()
{
   retro_start_emulator();

   if(!FRONTENDwantsExit)
      environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, 0);

   /* Were done here */
   co_switch(mainThread);

   /* Dead emulator, but libco says not to return */
   while(true)
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Running a dead emulator.\n");
      co_switch(mainThread);
   }
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_get_system_info(struct retro_system_info *info)
{
   info->library_name = "scummvm";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
   info->library_version = "1.9.0" GIT_VERSION;
   info->valid_extensions = "scummvm";
   info->need_fullpath = true;
   info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width = RES_W;
   info->geometry.base_height = RES_H;
   info->geometry.max_width = RES_W;
   info->geometry.max_height = RES_H;
   info->geometry.aspect_ratio = 4.0f / 3.0f;
   info->timing.fps = 60.0;
   info->timing.sample_rate = 44100.0;
}

void retro_init (void)
{
   struct retro_log_callback log;
   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

}

void retro_deinit(void)
{
}

void parse_command_params(char* cmdline)
{
   int j =0 ;
   int cmdlen = strlen(cmdline);
   bool quotes = false;

   /* Append a new line to the end of the command to signify it's finished. */
   cmdline[cmdlen] = '\n';
   cmdline[++cmdlen] = '\0';

   /* parse command line into array of arguments */
   for(int i=0; i<cmdlen; i++)
   {
      switch(cmdline[i])
      {
         case '\"' :
            if(quotes)
            {
               cmdline[i] = '\0';
               strcpy(cmd_params[cmd_params_num],cmdline+j);
               cmd_params_num++;
               quotes = false;
            }
            else
               quotes = true;
            j = i + 1;
            break;
         case ' ' :
         case '\n' :
            if(!quotes)
            {
               if(i != j && !quotes)
               {
                  cmdline[i] = '\0';                        
                  strcpy(cmd_params[cmd_params_num],cmdline+j);
                  cmd_params_num++;
               }
               j = i + 1;
            }
            break;
      }
   }
}

bool retro_load_game(const struct retro_game_info *game)
{
   const char* sysdir;
   const char* savedir;

   cmd_params_num = 1;
   strcpy(cmd_params[0],"scummvm\0");

   if (game)
   {
      /* Retrieve the game path. */
      char* path = strdup(game->path);
      char* gamedir = dirname(path);

      /* See if we are acting on a .scummvm file. */
      if (strstr(path, ".scummvm") != NULL)
      {
         // Retrieve the file data.
         FILE * gamefile;
         if (gamefile = fopen(game->path, "r"))
         {
            char filedata[400];
            fgets(filedata , 400, gamefile);
            fclose(gamefile);

            // Create a command line parameters using -p and the game name.
            char buffer[400];
            sprintf(buffer, "-p \"%s\" %s", gamedir, filedata);
            parse_command_params(buffer);
         }
      }
      else
      {
         // Retrieve the file data.
         FILE * gamefile;
         if (gamefile = fopen(game->path, "r"))
         {
            /* Acting on a ScummVM rom file. */
            char filedata[400];
            if (fgets(filedata, 400, gamefile) != NULL) {
               fclose(gamefile);
               char buffer[400];
               sprintf(buffer, "-p \"%s\" %s", gamedir, filedata);
               parse_command_params(buffer);
            }
         }
      }
   }

   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Mouse Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Mouse Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Mouse Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Mouse Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Mouse Button 1" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "Mouse Button 2" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Esc" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"ScummVM GUI" },
      { 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   /* Get color mode: 32 first as VGA has 6 bits per pixel */
#if 0
   RDOSGFXcolorMode = RETRO_PIXEL_FORMAT_XRGB8888;
   if(!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &RDOSGFXcolorMode))
   {
      RDOSGFXcolorMode = RETRO_PIXEL_FORMAT_RGB565;
      if(!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &RDOSGFXcolorMode))
         RDOSGFXcolorMode = RETRO_PIXEL_FORMAT_0RGB1555;
   }
#endif

#ifdef FRONTEND_SUPPORTS_RGB565
   enum retro_pixel_format rgb565 = RETRO_PIXEL_FORMAT_RGB565;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb565) && log_cb)
      log_cb(RETRO_LOG_INFO, "Frontend supports RGB565 -will use that instead of XRGB1555.\n");
#endif

   retro_keyboard_callback cb = {retroKeyEvent};
   environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);

   if(environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &sysdir))
      retroSetSystemDir(sysdir);
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_WARN, "No System directory specified, using current directory.\n");
      retroSetSystemDir(".");
   }

   if(environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &savedir))
      retroSetSaveDir(savedir);
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_WARN, "No Save directory specified, using current directory.\n");
      retroSetSaveDir(".");
   }

   if(!emuThread && !mainThread)
   {
      mainThread = co_active();
      emuThread = co_create(65536*sizeof(void*), retro_wrap_emulator);
   }

   return true;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
   return false;
}

void retro_run (void)
{
   if(!emuThread)
      return;

   /* Mouse */
   if(g_system)
   {
      poll_cb();
      retroProcessMouse(input_cb);
   }

   /* Run emu */
   co_switch(emuThread);

   if(g_system)
   {
      /* Upload video: TODO: Check the CANDUPE env value */
      const Graphics::Surface& screen = getScreen();
      video_cb(screen.pixels, screen.w, screen.h, screen.pitch);

      // Upload audio
      static uint32 buf[735];
      int count = ((Audio::MixerImpl*)g_system->getMixer())->mixCallback((byte*)buf, 735*4);
      audio_batch_cb((int16_t*)buf, count);
   }
}

void retro_unload_game (void)
{
   if(!emuThread)
      return;

   FRONTENDwantsExit = true;
   while(!EMULATORexited)
   {
      retroPostQuit();
      co_switch(emuThread);
   }

   co_delete(emuThread);
   emuThread = 0;
}

// Stubs
void retro_set_controller_port_device(unsigned in_port, unsigned device) { }
void *retro_get_memory_data(unsigned type) { return 0; }
size_t retro_get_memory_size(unsigned type) { return 0; }
void retro_reset (void) { }
size_t retro_serialize_size (void) { return 0; }
bool retro_serialize(void *data, size_t size) { return false; }
bool retro_unserialize(const void * data, size_t size) { return false; }
void retro_cheat_reset(void) { }
void retro_cheat_set(unsigned unused, bool unused1, const char* unused2) { }

unsigned retro_get_region (void) { return RETRO_REGION_NTSC; }

#if defined(GEKKO) || defined(__CELLOS_LV2__)
int access(const char *path, int amode)
{
   FILE *f;
   const char *mode;

   switch (amode)
   {
      // we don't really care if a file exists but isn't readable
      case F_OK:
      case R_OK:
         mode = "r";
         break;

      case W_OK:
         mode = "r+";
         break;

      default:
         return -1;
   }

   f = fopen(path, mode);

   if (f)
   {
      fclose(f);
      return 0;
   }

   return -1;
}
#endif
