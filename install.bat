call code.bat  C:\CBuilder5\Projects\AntiWebMiner\Debug\AntiWebMiner.exe 
copy C:\CBuilder5\Projects\AntiWebMiner\Debug\AntiWebMiner.exe D:\internet\greatis\new\antiwebminer
copy C:\CBuilder5\Projects\AntiWebMiner\LICENSE.TXT D:\internet\greatis\new\antiwebminer
copy C:\CBuilder5\Projects\AntiWebMiner\README.md  D:\internet\greatis\new\antiwebminer
copy C:\CBuilder5\Projects\AntiWebMiner\AntiWebMiner.ini  D:\internet\greatis\new\antiwebminer
copy C:\CBuilder5\Projects\AntiWebMiner\blacklist.txt  D:\internet\greatis\new\antiwebminer

"C:\Program Files\Inno Setup 554\Compil32.exe" /cc D:\internet\greatis\new\AntiWebMiner.iss
call D:\internet\greatis\new\cert\verisign16\sha-256\1signright.bat D:\internet\greatis\new\AntiWebMineSetup.exe
d:
cd \internet\greatis\new
zip antiwebminesetup.zip AntiWebMineSetup.exe
copy antiwebminesetup.zip C:\CBuilder5\Projects\AntiWebMiner

