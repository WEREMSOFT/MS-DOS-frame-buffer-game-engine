

mkdir bin
pushd src
cl -Zi ^
   /DWITH_MINIAUDIO ^
   /std:c++17 ^
   /I ..\..\libs\glfw\include ^
   /I ..\..\libs\pthreads\include ^
   /I ..\libs\soloud\include ^
   /I ..\libs\include ^
   main.c ^
   program\core\fonts\fonts.c ^
   program\core\graphics\graphics.c ^
   program\core\input\keyboard.c ^
   program\core\input\mouse.c ^
   program\core\shader\shader.c ^
   program\core\sprite\sprite.c ^
   program\core\stackAllocator\stack_allocator.c ^
   program\sound\sound.c ^
   ..\libs\soloud\src\audiosource\openmpt\soloud_openmpt_dll.c ^
   ..\libs\soloud\src\audiosource\wav\stb_vorbis.c ^
   ..\libs\soloud\src\audiosource\monotone\soloud_monotone.cpp ^
   ..\libs\soloud\src\audiosource\noise\soloud_noise.cpp ^
   ..\libs\soloud\src\audiosource\openmpt\soloud_openmpt.cpp ^
   ..\libs\soloud\src\audiosource\sfxr\soloud_sfxr.cpp ^
   ..\libs\soloud\src\audiosource\speech\darray.cpp ^
   ..\libs\soloud\src\audiosource\speech\klatt.cpp ^
   ..\libs\soloud\src\audiosource\speech\resonator.cpp ^
   ..\libs\soloud\src\audiosource\speech\soloud_speech.cpp ^
   ..\libs\soloud\src\audiosource\speech\tts.cpp ^
   ..\libs\soloud\src\audiosource\tedsid\sid.cpp ^
   ..\libs\soloud\src\audiosource\tedsid\soloud_tedsid.cpp ^
   ..\libs\soloud\src\audiosource\tedsid\ted.cpp ^
   ..\libs\soloud\src\audiosource\vic\soloud_vic.cpp ^
   ..\libs\soloud\src\audiosource\vizsn\soloud_vizsn.cpp ^
   ..\libs\soloud\src\audiosource\wav\dr_impl.cpp ^
   ..\libs\soloud\src\audiosource\wav\soloud_wav.cpp ^
   ..\libs\soloud\src\audiosource\wav\soloud_wavstream.cpp ^
   ..\libs\soloud\src\backend\alsa\soloud_alsa.cpp ^
   ..\libs\soloud\src\backend\miniaudio\soloud_miniaudio.cpp ^
   ..\libs\soloud\src\backend\wasapi\soloud_wasapi.cpp ^
   ..\libs\soloud\src\backend\winmm\soloud_winmm.cpp ^
   ..\libs\soloud\src\core\soloud.cpp ^
   ..\libs\soloud\src\core\soloud_audiosource.cpp ^
   ..\libs\soloud\src\core\soloud_bus.cpp ^
   ..\libs\soloud\src\core\soloud_core_3d.cpp ^
   ..\libs\soloud\src\core\soloud_core_basicops.cpp ^
   ..\libs\soloud\src\core\soloud_core_faderops.cpp ^
   ..\libs\soloud\src\core\soloud_core_filterops.cpp ^
   ..\libs\soloud\src\core\soloud_core_getters.cpp ^
   ..\libs\soloud\src\core\soloud_core_setters.cpp ^
   ..\libs\soloud\src\core\soloud_core_voicegroup.cpp ^
   ..\libs\soloud\src\core\soloud_core_voiceops.cpp ^
   ..\libs\soloud\src\core\soloud_fader.cpp ^
   ..\libs\soloud\src\core\soloud_fft.cpp ^
   ..\libs\soloud\src\core\soloud_fft_lut.cpp ^
   ..\libs\soloud\src\core\soloud_file.cpp ^
   ..\libs\soloud\src\core\soloud_filter.cpp ^
   ..\libs\soloud\src\core\soloud_misc.cpp ^
   ..\libs\soloud\src\core\soloud_queue.cpp ^
   ..\libs\soloud\src\core\soloud_thread.cpp ^
   ..\libs\soloud\src\c_api\soloud_c.cpp ^
   ..\libs\soloud\src\filter\soloud_bassboostfilter.cpp ^
   ..\libs\soloud\src\filter\soloud_biquadresonantfilter.cpp ^
   ..\libs\soloud\src\filter\soloud_dcremovalfilter.cpp ^
   ..\libs\soloud\src\filter\soloud_echofilter.cpp ^
   ..\libs\soloud\src\filter\soloud_fftfilter.cpp ^
   ..\libs\soloud\src\filter\soloud_flangerfilter.cpp ^
   ..\libs\soloud\src\filter\soloud_freeverbfilter.cpp ^
   ..\libs\soloud\src\filter\soloud_lofifilter.cpp ^
   ..\libs\soloud\src\filter\soloud_robotizefilter.cpp ^
   ..\libs\soloud\src\filter\soloud_waveshaperfilter.cpp ^
   ..\..\libs\glfw\lib-vc2022\glfw3dll.lib ^
   ..\..\libs\glfw\lib-vc2022\glfw3_mt.lib ^
   ..\..\libs\pthreads\lib\x64\pthreadVC2.lib ^
   opengl32.lib ^
   -o ../bin/main.exe
popd

xcopy ..\libs\glfw\lib-vc2022\glfw3.dll bin /Y
xcopy ..\libs\pthreads\dll\x64\pthreadVC2.dll bin /Y
xcopy assets bin\assets /E /H /C /I /Y

pushd bin
main.exe
popd
