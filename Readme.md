make all diyerek makefile'i çalistiriyoruz
sudo cp myshell /usr/local/bin/    diyerek olusan "myshell" binary dosyasini tasiyoruz
sudo chmod +x /usr/local/bin/myshell       calistirabilmek icin yetki veriyoruz 
direk erismek icin echo 'export PATH=$PATH:/usr/local/bin' >> ~/.bashrc yol verebiliriz ya da alias tanımlayabiliriz
bu asamadan sonra source ~/.bashrc 
myshell seklinde'de de kendi shell'imizi calistirabiliriz
2. yöntem daha sagliklidir 
Bu yöntem kalıcılık sağlamak içindir

Eğer kalıcılık sağlamak istemiyorsak bu yöntemi tavsiye ederiz:
İstersek make all komutunu çalıştırdıktan sonra aynı dizinde oluşan binary dosyasını (myshell)
direk olarak çalıştırabiliriz