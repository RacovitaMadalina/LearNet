
***  PROIECT RETELE - LEAR NET  ***

student: Racovita Madalina-Alina ; grupa: B2

1. Dependente: 
	      SQLite3
	      Qt Creator

2. Mod de rulare: Pentru a rula proiectul se vor deschide in Qt Creator proiectele: LearNetProj.pro si server.pro. 
Pentru a compila clientul si serverul se va folosi optiune "Built", iar pentru rulare optiune "Run".Rulam mai intai serverul, apoi clientul. 

3. Clientul ne ofera o interfata in care utilizatorul se poate conecta la aplicatie sau in cazul in care, nu are un cont se poate inregistra apasand butonul de sign up. Utilizatorii existenti in baza de date curenta "LearNet.db" (care contine 4 tabele) au parolele criptate cu ajutorul functiei MD5. De asemenea cei care se vor inregistra, vor avea parolele criptate. Dupa logare, se ajunge in search window unde utilizatorul poate cauta informatii despre retele introducand numarul aferent subcapitolului despre care acesta doreste  sa afle informatii. Butonul de Search ne va furniza informatiile aferente topicului. In schimb, daca dorim o cautare in functie de anumite cuvinte cheie putem apasa butonul de "Search with keywords".

Aplicatia ne va furniza o noua ferestra in care utilizatorul poate introduce un cuvant cheie despre care acesta doreste sa afle informatii. Iivor fi afisate titlurile topicurilor in care cuvantul se gaseste alaturi de numarul aparitiilor keyword-ului respectiv. 
Butonul de "Back to Search Window" ii va permite sa isi continue cautarile. Daca apasam butonul de Show forum va aparea fereastra unde 
utuilizatorul poate vedea conversatiile purtate de ceilalti utilizatori pe un topic anume. Va putea trimite mesaje doar in momentul in care a selectat deja un topic. Pentru a vedea lista updatata de mesaje trebuie sa apase din nou show forumul pentru acelasi topic in care a dorit sa trimita mesajul. 

Butonul de send a friend request, va deschide o fereastra in care utilizatorul poate vedea numele tuturor utilizatorilor care folosesc in prezent aplicatia (in partea stanga) prin apasarea butonului de "Show users". In partea dreapta daca utilizatorul apasa butonul de "Show friends" ii va aparea lista lui de prieteni. Utilizatorul poate trimite cereri e prietenie altor utilizatori, atata timp cat acestia nu sunt deja existenti in lista lui de prieteni. 

Mentionam ca relatia de prietenie este una mutuala. In fereastra de search exista butonul de Exit LearNet, care va descihde o noua ferestra in care clientul de poate deconecta de la contul curent prin apasarea butonului de "Sign out"  si conecta pe un alt cont sau poate inchide aplicatia prin apasarea butonului "Close the application".


4.Bibliografie: 
https://profs.info.uaic.ro/~computernetworks/	     
https://profs.info.uaic.ro/~computernetworks/files/NetEx/S12/ServerConcThread/servTcpConcTh2.c
https://profs.info.uaic.ro/~computernetworks/files/NetEx/S12/ServerConcThread/cliTcpNr.c
http://www.zedwood.com/article/cpp-md5-function
https://katecpp.wordpress.com/2015/08/28/sqlite-with-qt/
http://doc.qt.io/qtcreator/
http://doc.qt.io/qtcreator/creator-debugging.html
https://www.sqlite.org/cli.html
https://stackoverflow.com/questions/2523765/qstring-to-char-conversion
http://doc.qt.io/qt-5/sql-connecting.html
http://doc.qt.io/archives/qt-4.8/dialogs.html
https://www.youtube.com/watch?v=pzzccU9mErg&t=316s
https://www.youtube.com/watch?v=wUH_gu2HdQE
