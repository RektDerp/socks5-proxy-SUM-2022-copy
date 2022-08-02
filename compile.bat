@ECHO OFF
set root=%CD%
rmdir build /S /Q
mkdir build/bin

set /p Qt=Enter Qt major version [Qt5 ^/ Qt6]^>
set /p Qtdir=Enter Qt installation root^>
if NOT EXIST "%Qtdir%" ( set Qtdir=C:\Qt\6.3.1\mingw_64\)
set mQtdir=%Qtdir:\=/%
echo %mQtdir%
::GOTO COPYFILES
set dlls=libgcc_s_seh-1.dll %Qt%OpenGL.dll %Qt%Quick.dll libstdc++-6.dll %Qt%Qml.dll  %Qt%QuickLayouts.dll libwinpthread-1.dll  %Qt%QmlModels.dll  %Qt%QuickTemplates2.dll %Qt%Core.dll %Qt%QmlWorkerScript.dll %Qt%Sql.dll %Qt%Gui.dll %Qt%QuickControls2.dll %Qt%Svg.dll %Qt%Network.dll %Qt%QuickControls2Impl.dll

cmake -S . -B build -D Qt=%Qt% -D Qtdir=%mQtdir% -G "MinGW Makefiles"
if %errorlevel%==0 (
  cd build
  cmake --build . -j 8 && GOTO COPYFILES
) else (
  echo configure failed
  GOTO END
)

:COPYFILES
cd %root%
mkdir build\bin\statistics
mkdir build\bin\statistics\imageformats
mkdir build\bin\statistics\platforms
mkdir build\bin\statistics\sqldrivers
mkdir build\bin\statistics\QtQml
mkdir build\bin\statistics\QtQuick
xcopy %Qtdir%\plugins\imageformats %root%\build\bin\statistics\imageformats /S /E /Q
xcopy %Qtdir%\plugins\platforms %root%\build\bin\statistics\platforms /S /E /Q
xcopy %Qtdir%\plugins\sqldrivers %root%\build\bin\statistics\sqldrivers /S /E /Q
xcopy %Qtdir%\qml\QtQml %root%\build\bin\statistics\QtQml /S /E /Q
xcopy %Qtdir%\qml\QtQuick %root%\build\bin\statistics\QtQuick /S /E /Q
for %%G in ( %dlls% ) do ( xcopy %Qtdir%\bin\%%G %root%\build\bin\statistics\ /S /E /Q /I )
xcopy %root%\config.txt %root%\build\bin\config.txt /S /E /Q

:END
pause
