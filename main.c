#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
/*****************************************************************************
* testleri yaptırmak için //'i kaldır
******************************************************************************/

//#define TESTLERYAP 1
//#define TRACEYAP 1

/*****************************************************************************
* struct nokta
*
* x,y,z koordineleri, float
* r,g,b değerleri (varsa, yoksa sıfır) int.
*
* };
*
* typedef ifadesi:  struct nokta_tag ifadesini nokta yerine geçirir (takma ad)
* yani struct nokta_tag = nokta
******************************************************************************/
typedef struct nokta_tag {
  float x,y,z;
  int r,g,b;
} nokta;

/*****************************************************************************
*  struct dosya
* dosya_ismi, string (sonunda sıfır)
* renk_var_mi renk bildisi varsa 1 yoksa 0 : int
* nokta_sayisi dosyadaki nokta sayısı: int
* kaynak_ascii - girdi ascii ise 1 yoksa binary için 0
* noktalar
*
*
* renk_var_mi eşitse 1 ' e hata meseajı yok eşitse 0'a hata mesajı var.
******************************************************************************/
typedef struct dosya_tag {
  char * dosya_ismi;
  int renk_var_mi;
  int nokta_sayisi;
  bool kaynak_ascii;
  nokta * noktalar;
} dosya;


/******************************************************************************
* is_file
* string -> bool
*
* path bir dosyaya giden bir path ise true yoksa false
*
* struct stat buf sys/stat.h kütüphanesinin içinde
* struct stat buf : Dosya özniteliklerini döndürmek için kullanılır
* kullanımı : stat(*path name , struct stat *statbuf)
* S_ISREG() : Normal bir dosya mı bunu kontrol eder.
* st_mode : dosya türünü ve modunu içerir.
* S_ISREG(buf.st_mode) : stat içerisinde gönderilen path dosyaya giden path
* ise true değilse false döner.
******************************************************************************/
bool is_file(const char* path) {
  struct stat buf;
  stat(path, &buf);
  return S_ISREG(buf.st_mode);
}


/******************************************************************************
* file_ext_ayni
* string string -> bool
*
* path bir dosya ise ve sonunda noktalı ext varsa true yoksa false döndürüyor
*
* yukarıdaki fonksiyon burada çağırarak (is_file) path'ın bir dosya olup olamadığı
* kontrol edildi.
*
* strlen(path) > strlen(ext) : veriler.nkt > nkt olmalı (kontrol)
* ext path içinde arandığı için path dan kısa olmalı
*
* is_file(path) : Bu fonksiyondan  1 dönerse bu path bir dosyadır (kontrol)
*
* [strlen(path) - strlen(ext) -1] veriler.nkt 11 harkfli nkt 3 harfli 11-3 = 8
* -1 ' in nedeni string dizilerinin ilk harfi 0 hücresindedir 8-1 = 7
* '.' karakteri 7. hücrede ise (kontrol)
*
* !strcmp(path + strlen(path)-strlen(ext), ext) : Verilen iki dizigiyi alfabetik olarak
* karakter karakter birbiri ile karşılaştırır. İkisi biribirinen eşit ise 0 döner.
* Bu yüzden başında ! (not) ifadesi vardır return ile 1 dönerse true olarak algılanır.
* 0 dönseydi yanlış olarak algılanırdı.
*
* path + strlen(path)-strlen(ext) = path[strlen(path) - strlen(ext)]:
* verilen.nkt 11 harfli .nkt 4 harfli 11-4 = 7 path[7] == '.' ext[0] = '.'
* olduğu için dosyanın isminin uzunluğundan noktalı ext çıkartılırsa bulunduğumuz konum
* '.' olur nokta dahil uzantıyla karakter karakter aynı ise strcmp 0 döner.
* Yani dosya isminin uzantısının .nkt olup olmadığı kontrol edilir.
*
******************************************************************************/

bool file_ext_ayni (char * path, char* ext) {
  return  (strlen(path) > strlen(ext))
           && is_file(path)
           && (path[strlen(path)-strlen(ext)-1] == '.')
           && !strcmp(path + strlen(path)-strlen(ext), ext);
}

/******************************************************************************
* dosya_yapi_yarat
* string bool int -> dosya
*
* verilen isim, renk özelliği ve nokta sayısıyla bir dosya veri yapısı yarat
*
* Fonksiyon struct dosya_tag tipinde yani geri bir struct dönebilir.
* Girdiler : string isim , bool renk_var , int n
* d->dosya_ismi = isim;  Dosyanın ismi d struct ı içine dosya_ismi ne atıldı.
* d->renk_var_mi = renk_var; Boolen bir değer 1 ya da 0 renk_var_mı içine atıldı.
* d->nokta_sayisi = n; nokta sayisi d structına atıldı.
* nokta sayısı kadar alan malloc ile elde edildi.
* for ile tüm noktaların içine 0 değeri atandı (yapı yaratıldı.)
* Boyutu önceden belli olmadığı için malloc ile hafıza ayırt etmek zorundayız.
* Fonksiyon, sonuç olarak, yaratılan dosya tipinden veri yapısının pointerini döndürüyor.
******************************************************************************/
dosya * dosya_yapi_yarat (char * isim, bool renk_var, int n) {
  dosya * d = malloc (sizeof (dosya));
  d->dosya_ismi = isim;
  d->renk_var_mi = renk_var;
  d->nokta_sayisi = n;
  d->noktalar = malloc (n * sizeof (nokta));
  for (int i = 0; i < n; i++) {
    d->noktalar[i].x = 0;
    d->noktalar[i].y = 0;
    d->noktalar[i].z = 0;
    d->noktalar[i].r = 0;
    d->noktalar[i].g = 0;
    d->noktalar[i].b = 0;
  }
  return d;
}

/**************************************************************************
* Void fonksiyonu olduğu için geri değer döndürmez.
* Girdi : nokta (struct nokta_tag), FILE *
* Fonksiyon nokta verilerini verilen strem'e yazar.
*
****************************************************************************/

void nokta_yaz(nokta * nk, FILE * cikti) {
  fprintf(cikti, "\n %f %f %f %d %d %d\n", nk->x, nk->y, nk->z, nk->r, nk->g, nk->b);
}

/**************************************************************************
* Void fonksiyonu olduğu için geri değer döndürmez.
* Girdi : nokta (struct nokta_tag), FILE *
* Fonksiyon nokta verilerini verilen strem'e yazar.
*
****************************************************************************/

void nokta_yaz_renksiz(nokta * nk, FILE * cikti) {
  fprintf(cikti, "\n %f %f %f\n", nk->x, nk->y, nk->z);
}

/****************************************************************************
* dosya_yapi_yaz :
* dosya ,  FILE * -> dosya
* Fonksiyon, sonuç olarak, yazılan dosya tipinden veri yapısının pointerini döndürüyor.
* (TEST Fonksiyonu)
* Dosyanın ismini ve dosya içindeki noktaların renk bilgisini yazdırır.
* for döngüsü ile tüm noktalar veri de kayıtlı olan nokta sayısına kadar yazdırırlır.
* nokta_yaz (&(d->noktalar[i])); dosya yapısı içerisindeki nokta yapısının elemanları verilen stream'e yazdırlır.
* Adres gönderilmesinin nedeni dosya yapısı içerisinde nokta * noktalar olmasıdır.
* dosya yapısı geri döndürülür.
*
******************************************************************************/
#ifdef TESTLERYAP
dosya * dosya_yapi_yaz (dosya * d, FILE * cikti) {
  fprintf(cikti, "\nFile name %s", d->dosya_ismi);
  fprintf(cikti, "\nRenk %s", d->renk_var_mi?"Var":"Yok");
  for (int i = 0; i < d->nokta_sayisi; i++) {
    nokta_yaz(&(d->noktalar[i]), cikti);
  }
  fprintf(cikti, "\n");
  return d;
}
#endif

/****************************************************************************
* malloc tarafından bellekten alınan alan silinerek sisteme geri iade edilir.
* Void fonksiyonu olduğu için döndürdüğü değer yok
* Girdi : dosya
*****************************************************************************/
void free_dosya (dosya * d) {
  free (d->noktalar);
  free (d);
}

/**************************************************************************************
satir_yaz
dosyanın içerisine bir satır yazar.
Girdi : FILE *cikti,kombin_1,kombin_2,kombin_3


*************************************************************************************/
void satir_yaz(FILE *cikti,float kombin_1,float kombin_2,float kombin_3){


 fprintf(cikti,"%f %f %f\n",kombin_1,kombin_2,kombin_3);


}
/**************************************************************************************

void kup_kenarlari
Fonksiyon void olduğu için bir geri dönüş değeri döndürmeyecek.
Girdi : dosya * d, FILE* cikti


Dosya veri yapısından nokta veri yapısının içerisindeki noktaları kullanarak :
"Tüm noktaları içine alacak bir küpün kenar nokta bilgileri"
bilgisine erişilecek.

Kenar nokta bilgilerini bulduktan sonra cikti dosyası içerisine değerler yazdırılacak.

* Noktalar dizisinin sıfırıncı elemanını en büyük ve en küçük olarak atadım.
* Dosyadaki eleman sayısı kadar dönen döngüde elemanlar sıfırıncı elemandan
  buyuk mü küçük mü olduğuna bakılır ve eğer büyükse en büyüğe atanır küçükse
  en küçüğe atanır.

****************************************************************************************/
void kup_kenarlari(dosya *d, FILE *cikti){

  float enBuyukx, enBuyuky, enBuyukz;
  float enKucukx, enKucuky, enKucukz;

  enBuyukx = enKucukx =  d->noktalar[0].x;
  enBuyuky = enKucuky =  d->noktalar[0].y;
  enBuyukz = enKucukz =  d->noktalar[0].z;

  for(int i = 1 ; i < d->nokta_sayisi ; i++){
    enBuyukx = fmax(d->noktalar[i].x, enBuyukx);
    enKucukx = fmin(d->noktalar[i].x, enKucukx);

    enBuyuky = fmax(d->noktalar[i].y, enBuyuky);
    enKucuky = fmin(d->noktalar[i].y, enKucuky);

    enBuyukz = fmax(d->noktalar[i].z, enBuyukz);
    enKucukz = fmin(d->noktalar[i].z, enKucukz);

  }
   fprintf(cikti,"\nSEÇİM 3: \n\n");
   satir_yaz(cikti,enKucukx,enKucuky,enBuyukz);
   satir_yaz(cikti,enKucukx,enBuyuky,enBuyukz);
   satir_yaz(cikti,enBuyukx,enKucuky,enBuyukz);
   satir_yaz(cikti,enBuyukx,enBuyuky,enBuyukz);
   satir_yaz(cikti,enKucukx,enKucuky,enKucukz);
   satir_yaz(cikti,enKucukx,enBuyuky,enKucukz);
   satir_yaz(cikti,enBuyukx,enKucuky,enKucukz);
   satir_yaz(cikti,enBuyukx,enBuyuky,enKucukz);

}
/**************************************************************************************
uzaklik_bul
İki nokta arasındaki mesafeyi aşağıdaki formül ile hesaplar ve hesapladığı
değeri döndürür.
Formül -> uzaklık = √(x1 - x2)^2 + (y1 - y2)^2 + (z1 - z2)^2
nokta * nokta * -> float


**************************************************************************************/

float uzaklik_bul(nokta *deger1, nokta *deger2){

   float uzaklik = sqrt(pow((deger2->x - deger1->x),2) +
                        pow((deger2->y - deger1->y),2) +
                        pow((deger2->z - deger1->z),2));
   return uzaklik;
}
/*************************************************************************************
kure_noktalari
Girdi : dosya *d , FILE *cikti
Merkezinin 3 boyutlu koordinatı ve yarıçapı bilgileri kullanıcıdan istenilen bir kürenin
içinde kalan noktaların bilgilerini ve nokta numaralarını verir.

Noktanın kürenin içerisinde olması için : M(a,b,c) olduğunu varsayarsak
((x-a)^2 + (y-b)^2 + (z-c)^2)^1/2 < r olması lazım.
Bu yüzden merkez noktası ile arasındaki uzaklık hesaplanan noktanan dönenen değer
(uzaklik_bul() fonksiyonundan dönen değer)  eğer kullanıcıdan alınan yarıçaptan küçük ise
bu nokta kürenin içerisindedir.


**************************************************************************************/

void kure_noktalari(dosya *d, FILE *cikti,int renk){

   float yaricap;
   nokta merkez;

   printf("Kürenin yarı çapını giriniz: ");
   scanf("%f",&yaricap);

   printf("Küreninin merkezinin x koordinatını giriniz: ");
   scanf("%f",&merkez.x);

   printf("Küreninin merkezinin y koordinatını giriniz: ");
   scanf("%f",&merkez.y);

   printf("Küreninin merkezinin z koordinatını giriniz: ");
   scanf("%f",&merkez.z);

   fprintf(cikti,"\nSEÇİM 4: \n\n");
   fprintf(cikti,"Kullanıcıdan alınan r değeri : %f\n",yaricap);
   fprintf(cikti,"Kullanıcıdan alınan x değeri : %f\n",merkez.x);
   fprintf(cikti,"Kullanıcıdan alınan y değeri : %f\n",merkez.y);
   fprintf(cikti,"Kullanıcıdan alınan z değeri : %f\n",merkez.z);

   for(int i = 0 ; i < d->nokta_sayisi ; i++){
      if(uzaklik_bul(&d->noktalar[i],&merkez) < yaricap){
         fprintf(cikti,"%d.nokta : ",i+1);
         if(renk){
            nokta_yaz(&d->noktalar[i], cikti);
         }
         else{
           nokta_yaz_renksiz(&d->noktalar[i],cikti);
         }

      }
   }
}

/************************************************************************************

yakın_uzak_noktalar
Girdi : dosya *d , FILE *cikti
Dosyaya birbirine en yakın nokta ve en uzak noktaların bilgilerini ve nokta
numaralarını yazar.

Birbirine en uzak olan noktalar aralarındaki uzaklığın en büyük olduğu noktalardır.
Birbirine en yakın olan noktalar aralarındaki uzaklığın en az olduğu noktalardır.

*************************************************************************************/

void yakin_uzak_noktalar(dosya *d, FILE *cikti, int renk){

   float en_yakin = FLT_MAX; //100000000000000000;
   float en_uzak = 0;
   int yakin_yer1= 0;
   int yakin_yer2 = 0;
   int uzak_yer1 = 0;
   int uzak_yer2 = 0;

   for(int i = 0 ; i < d->nokta_sayisi; i++){
      for(int j = i+1 ; j < d->nokta_sayisi ; j++){
        #ifdef TRACEYAP
        // printf("en yakin testi, i = %d, j = %d \n\n", i, j);
        #endif
         if(uzaklik_bul(&d->noktalar[i],&d->noktalar[j]) < en_yakin){
            en_yakin = uzaklik_bul(&d->noktalar[i],&d->noktalar[j]);
            yakin_yer1 = i;
            yakin_yer2 = j;
         }
         if(uzaklik_bul(&d->noktalar[i],&d->noktalar[j]) > en_uzak){
            en_uzak = uzaklik_bul(&d->noktalar[i],&d->noktalar[j]);
            uzak_yer1 = i;
            uzak_yer2 = j;
         }
      }
   }
   #ifdef TRACEYAP
   printf("En Yakın=%f En uzak=%f buldum - şimdi basacağım\n\n",en_yakin,en_uzak);
   #endif
   fprintf(cikti,"\nSEÇİM 2: \n");
   if(renk){
      fprintf(cikti,"\nEn yakın %d.nokta : ",yakin_yer1);
      nokta_yaz(&d->noktalar[yakin_yer1], cikti);
      fprintf(cikti,"\nEn yakın %d.nokta : ",yakin_yer2);
      nokta_yaz(&d->noktalar[yakin_yer2], cikti);
      fprintf(cikti,"\nEn uzak %d.nokta: ",uzak_yer1);
      nokta_yaz(&d->noktalar[uzak_yer1], cikti);
      fprintf(cikti,"\nEn uzak %d.nokta: ",uzak_yer2);
      nokta_yaz(&d->noktalar[uzak_yer2], cikti);

   }
   else{
     fprintf(cikti,"\nEn yakın %d.nokta : ",yakin_yer1);
     nokta_yaz_renksiz(&d->noktalar[yakin_yer1], cikti);
     fprintf(cikti,"\nEn yakın %d.nokta : ",yakin_yer2);
     nokta_yaz_renksiz(&d->noktalar[yakin_yer2], cikti);
     fprintf(cikti,"\nEn uzak %d.nokta: ",uzak_yer1);
     nokta_yaz_renksiz(&d->noktalar[uzak_yer1], cikti);
     fprintf(cikti,"\nEn uzak %d.nokta: ",uzak_yer2);
     nokta_yaz_renksiz(&d->noktalar[uzak_yer2], cikti);

   }

}

/*************************************************************************************
uzaklik_ortalaması
Girdi : dosya *d , 1
1
FILE *cikti
Dosyaya her bir noktanın birbirlerine olan uzaklıklarının ortalamasını yazar.

***************************************************************************************/

void uzaklik_ortalamasi(dosya *d, FILE *cikti){
   double mesafe = 0;
   double sayac = 0;
   for(int i = 0 ; i < d->nokta_sayisi ; i++){
      for(int j = i + 1 ; j < d->nokta_sayisi ; j++){
          mesafe += uzaklik_bul(&d->noktalar[i],&d->noktalar[j]);
          sayac++;
      }
   }
   fprintf(cikti,"\nSEÇİM 5: \n\n");
   fprintf(cikti,"Ortalama: %f",mesafe/sayac);
}


/******************************************************************************
* doğrulanmış_dosya_islet
* dosya seçimler FILE* FILE *-> dosya
*
* bir file path alarak dosyayı işlet
******************************************************************************/
dosya * dogrulanmis_dosya_islet(dosya * d, bool * secimler, FILE * girdi, FILE * cikti, int renk) {
  if (secimler[2]) {
      #ifdef TRACEYAP
      printf("secim %d yapılacak\n\n", 2);
      #endif
      yakin_uzak_noktalar(d,cikti,renk);
  }
  if (secimler[3]) {
      #ifdef TRACEYAP
      printf("secim %d yapılacak\n\n", 3);
      #endif
      kup_kenarlari(d,cikti);
  }
  if (secimler[4]) {
      #ifdef TRACEYAP
      printf("secim %d yapılacak\n\n", 4);
      #endif // TRACEYAP
      kure_noktalari(d,cikti,renk);
  }
  if (secimler[5]) {
      #ifdef TRACEYAP
      printf("secim %d yapılacak\n\n", 5);
      #endif // TRACEYAP
      uzaklik_ortalamasi(d,cikti);
  }
  return d;
}



/******************************************************************************
* dosya_islet
* string -> void
*
* bir file path alarak dosyayı işlet
******************************************************************************/
void dosya_islet (char * full_path, bool * secimler) {
  printf("Dosya %s işletiliyor...\n ", full_path);
  char *dizi;
  int renk;
  size_t uzunluk = 1000;
  char cikti_path[200];
  strcpy(cikti_path, full_path);
  strcat(cikti_path,".out");
  FILE *girdi = fopen(full_path,"r");
  if (!girdi) {
     printf("Girdi dosyası açılamadı %s \n\n", full_path );
     return;
  }
  FILE *cikti = fopen(cikti_path,"w");
  if (!cikti) {
     printf("Çikti dosyası açılamadı %s \n\n", cikti_path );
     return;
  }
  fprintf(cikti,"SEÇİM 1 : \n\n");


  dizi = (char *)malloc(uzunluk * sizeof(char));
  if (!dizi ) {
     printf("Malloc olamadı\n\n");
     return;
  }
  #ifdef TRACEYAP
  printf("1 geldim\n\n");
  #endif
  fgets(dizi,uzunluk,girdi);
  #ifdef TRACEYAP
  printf("2 geldim okudum %s\n\n", dizi);
  #endif // TRACEYAP

  bool dogru_mu_yorum;
   if(strcmp(dizi,"# Noktalar dosya format\n") == 0){
      dogru_mu_yorum = true;
   }
   else{
      fprintf(cikti,"Yorum satırı yanlış\n");
      dogru_mu_yorum = false;
   }

  fgets(dizi,uzunluk,girdi);
  fgets(dizi,uzunluk,girdi);
  #ifdef TRACEYAP
  printf("3 geldim okudum %s\n\n", dizi);
  #endif // TRACEYAP
  bool renk_yazi_dogru;
  if(strcmp(dizi,"ALANLAR x y z r g b\n") == 0) {
    renk = 1;
    renk_yazi_dogru = true;
  } else if (strcmp(dizi,"ALANLAR x y z\n") == 0) {
    renk = 0;
    renk_yazi_dogru = true;
  } else {
     fprintf(cikti,"Renk yok veya yanlış yazılmış\n");
     renk_yazi_dogru = false;
     return;
  }
  fgets(dizi,uzunluk,girdi);
  #ifdef TRACEYAP
  printf("4 geldim okudum %s\n\n", dizi);
  #endif
  char *endptr;
  long nokta_sayi;
  for(int i = 0 ; i < strlen(dizi); i++){
    if(dizi[i] == ' '){
       nokta_sayi = strtol(&dizi[i+1],&endptr,10);
       break;
    }
  }

  fgets(dizi,uzunluk,girdi);
  bool ascii;
  bool format_hatasi;
  if(strcmp(dizi,"DATA ascii\n") == 0){
    ascii = true;
    format_hatasi = true;
  } else if (strcmp(dizi,"DATA binary\n") == 0) {
    ascii = false;
    format_hatasi = true ;
  } else {
    fprintf(cikti,"Dosya format hatası\n");
    format_hatasi = false;
    return;
  }

  int sayac = 0;

    dosya * d = dosya_yapi_yarat(full_path,renk,nokta_sayi);
    int i ;
    while(!feof(girdi)){
    if(ascii){
       if(renk) {
          fscanf(girdi,"%f %f %f %d %d %d\n",&d->noktalar[i].x,&d->noktalar[i].y,&d->noktalar[i].z,
                                             &d->noktalar[i].r,&d->noktalar[i].g,&d->noktalar[i].b);
      } else {
          fscanf(girdi,"%f %f %f\n",&d->noktalar[i].x,&d->noktalar[i].y,&d->noktalar[i].z);
      }


    }else{
      if(renk){
         fread(&d->noktalar[i].x,sizeof(float),1,girdi);
         fread(&d->noktalar[i].y,sizeof(float),1,girdi);
         fread(&d->noktalar[i].z,sizeof(float),1,girdi);
         fread(&d->noktalar[i].r,sizeof(float),1,girdi);
         fread(&d->noktalar[i].g,sizeof(float),1,girdi);
         fread(&d->noktalar[i].b,sizeof(float),1,girdi);

      }
      else{

         fread(&d->noktalar[i].x,sizeof(float),1,girdi);
         fread(&d->noktalar[i].y,sizeof(float),1,girdi);
         fread(&d->noktalar[i].z,sizeof(float),1,girdi);
      }
    }
     sayac++;
     i++;
    }

   bool dogru_mu_sayac;
   if(ascii){
      if(nokta_sayi == sayac){
         dogru_mu_sayac = true;
      }
   else {
        fprintf(cikti,"Dosyadaki nokta sayisi yanlış\n");
           dogru_mu_sayac = false;
        }
   }
   else{

        if(nokta_sayi == sayac -1){
           dogru_mu_sayac = true ;

        }
        else{
          fprintf(cikti,"Dosyadaki nokta sayisi yanlış\n");
          dogru_mu_sayac = false;

        }

   }

  if(renk_yazi_dogru && format_hatasi && dogru_mu_sayac && dogru_mu_yorum){

    fprintf(cikti,"Dosya Uyumludur.\n");

  }

  #ifdef TESTLERYAP
  printf("Secimler : "); // test kodu
  for (int i = 0 ; i < 7; i++) {
    if (secimler[i]) printf ("%d, ", i);
  }
  printf("\n\n");
  #endif

  fseek(girdi,0L,SEEK_SET);
  fgets(dizi,uzunluk,girdi);
  fgets(dizi,uzunluk,girdi);
  fgets(dizi,uzunluk,girdi);
  fgets(dizi,uzunluk,girdi);
  fgets(dizi,uzunluk,girdi);

  i = 0;
  while(!feof(girdi)) {
    #ifdef TRACEYAP
    //printf("5 geldim okudum satır %d\n\n", i);
    #endif
    if(ascii){

      if(renk) {
        fscanf(girdi,"%f %f %f %d %d %d\n",&d->noktalar[i].x,&d->noktalar[i].y,&d->noktalar[i].z,
                                           &d->noktalar[i].r,&d->noktalar[i].g,&d->noktalar[i].b);
      }else{
        fscanf(girdi,"%f %f %f\n",&d->noktalar[i].x,&d->noktalar[i].y,&d->noktalar[i].z);
      }
    } else {
      if (renk) {
        fread(&d->noktalar[i].x,sizeof(float),1,girdi);
        fread(&d->noktalar[i].y,sizeof(float),1,girdi);
        fread(&d->noktalar[i].z,sizeof(float),1,girdi);
        fread(&d->noktalar[i].r,sizeof(int),1,girdi);
        fread(&d->noktalar[i].g,sizeof(int),1,girdi);
        fread(&d->noktalar[i].b,sizeof(int),1,girdi);
      } else {
        fread(&d->noktalar[i].x,sizeof(float),1,girdi);
        fread(&d->noktalar[i].y,sizeof(float),1,girdi);
        fread(&d->noktalar[i].z,sizeof(float),1,girdi);
      }
    }
    i++;
  }
  dogrulanmis_dosya_islet(d,secimler,girdi,cikti,renk);
  free_dosya(d);
  fclose(girdi);
  fclose(cikti);
}
/******************************************************************************
* dosya_filtre_islet
* string string-> void
*
* bir path ve dosya ismi uygun olduğunda dosya işlet
*
* path ve dosya ismi fazla uzunsa hata verir.
*
* full_path = path + isim eğer bu iki stringin uzunluğunun toplamı 512 -1 den uzunsa hata verir
* -1 in nedeni son hücrenini '\0' olması
*
* path in sonu '/' karakteri ile bitmiyorsa strcat ile '/' ekler
* ismi path in sonuna ekleyerek full_path elde edilir (yani erişimin sağlanacağı tam path)
* file_ext_ayni fonksiyonu çağrılır bu fonksiyon bool tipinde olduğu için
* 1 veya 0 döndürür eğer dönen değer bir ise dosyanın uzantısı nkt (istediğimiz dosyadir)
* ve dosya_islet fonksiyonu çalışır.
*
******************************************************************************/

void dosya_filtre_islet (char * path, char * isim, bool * secimler) {
  char full_path[512];
  #ifdef TESTLERYAP
  printf ("Unfiltered Path = %s, Dosya ismi = %s\n", path, isim); //test kodu
  #endif
  if (strlen(path) + strlen(isim) > 512 -1) return; //hata: path fazla uzun....
  strcpy(full_path,path);
  if (full_path[strlen(full_path) -1] != '/') {
     strcat(full_path,"/");
  }
  strcat(full_path,isim);
  if (file_ext_ayni(full_path,"nkt")) {
     dosya_islet(full_path, secimler);
  }
}


/**************************************************************************************
menu()
bool * -> void

Verilen dizi 7 elemanlık olmalı

Kullanıcadan girdi alınır ve bu girdi numaralarına göre ve sonuçları paramtre olarak
verilen 7 elemanlık bir bool dizisine yazar.



***************************************************************************************/
void menu(bool * secimler){
   int son_secim = 0;
   for (int i=0 ; i < 6 ; i++) {
     secimler[i] = false;
   }
   while ( son_secim < 5) {
     printf("1.Dosya kontrolü \n");
     printf("2.En Yakın/Uzak Noktalar\n");
     printf("3.Küp \n");
     printf("4.Küre \n");
     printf("5.Nokta Uzaklıklarının Ortalaması \n\n");
     printf("Bir seçim yapınız [1,2,3,4,5,6] 5 seçilecekse, son seçim olsun, secimleri bitirmek için 6'ya bas\n");
     scanf("%d",&son_secim);
     secimler[son_secim] = true;
   }
}

/**************************************************************************************
* dosya_yapi_yarat() fonksiyonu ile renk bilgileri içersinde olan yapı yaratıldı
* 10 tane noktaya 0 değeri atandı.
*
* dosya_yapı_yaz() fonksiyonun girdisi dosya , dosya_yapi_yarat() fonksiyonundan aldığı dosya
* yapısını print_nokta ile yazdı.
*
* free_dosya() fonksiyonu girdi olarak aldığı dosya için alınan alanı geri verdi
*
* argc komut satırından gelen kelimelerin sayısı : Eğer bu sayı ikiye eşit değilse
* Komut satırında, tek parametre olarak, klasörün path'ı vermek gerek yazısını yazıp çıkış yapar
* EXIT_FAILURE : hatalı sonuçlanma
*  dir = opendir (argv[1]); komut satırından gelen birinci string ile klasörü aç
*  de = readdir (dir); dir i okumak için aç eğer boş değilse
*  dosya_filtre_islet (argv[1],de->d_name); dosyanın ismini argv[1] ' e yaz.
********************************************************************************************/

int main (int argc, char** argv) {
  DIR *dir;
  struct dirent *de;
  bool secimler[7];
  #ifdef TESTLERYAP
  free_dosya(dosya_yapi_yaz(dosya_yapi_yarat("./main.c /home/hilal/Code::Blocks/proje_1/", 1, 10)));//test kodu
  #endif
  if (argc != 2) {
    printf("\n\nKomut satırında, tek parametre olarak, klasörün path'ı vermek gerek\n\n");
    return EXIT_FAILURE;
  }
  dir = opendir (argv[1]);
  if (dir == NULL) {
    printf("\nKlasör %s açılamadı\n\n", argv[1]) ;
    return EXIT_FAILURE;
  }
  menu(secimler);
  de = readdir (dir);
  while (de != NULL) {
    dosya_filtre_islet (argv[1],de->d_name, secimler);
    de = readdir (dir);
  }
  closedir (dir);
}

