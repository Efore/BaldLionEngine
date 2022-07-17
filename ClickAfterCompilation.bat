if not exist bin goto failstate
if not exist bin\Debug-windows-x86_64\BaldLionEditor\assets mkdir bin\Debug-windows-x86_64\BaldLionEditor\assets
xcopy /s BaldLionEditor\assets bin\Debug-windows-x86_64\BaldLionEditor\assets
xcopy BaldLionEngine\vendor\assimp\lib\assimp-vc142-mtd.lib bin\Debug-windows-x86_64\BaldLionEditor
goto commonexit

:failstate
echo Please click on "GenerateProjects" first and then compile the generated solution before clicking on this
goto commonexit

:commonexit
pause
