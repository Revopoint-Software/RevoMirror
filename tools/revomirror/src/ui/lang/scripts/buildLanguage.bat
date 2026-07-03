python  version 2>NUL
if errorlevel 0 goto buildLanguage

echo please install Python first
pause
exit

:buildLanguage
python language.py ../language.xlsx ../trans
pause
