
// step 1 SCAN (DONE)

// arg1 adapted to accept first arg as device id syntax
// acr122_usb:001:013

// arg2 is file to write to use this info to map to proper file
// /path/to/anyfile.txt

// arg3 server url

// arg4 leds to flicker, either 0, 4 or 8

// arg5 extra body parameters (ex.: like=0 or like=1)

// EXAMPLE COMMAND 

// sudo ./poll-d3 acr122_usb:001:005 file.txt http://www.google.be 0 like=1

// step 2 HTTP POST (MOSTLY DONE)

// update for args
// info from http://stackoverflow.com/questions/11471690/curl-h-no-such-file-or-directory

// now add lcurl as lib when compiling

// step 3 LEDS

// led info from https://code.google.com/p/boblight/wiki/LPD8806_on_Raspberry_Pi

// don't forget to load the spi module with
// modprobe spi_bcm2708

// now compile with -lm and lpd

// added extra arg for leds to flicker

// COMPILATION

// gcc -o poll-d3 poll-d3.c -lnfc -lcurl lpd8806led.o -lm

// step 4 config (TODO)
 
// load from file
// bootscript
// ...

// TODO OPEN POINTS

// TODO watchdog for the script
// TODO load test
// TODO check if usb device IDs remain after reboot !?

// USAGE

// run the 3 scripts (per reader one) using sudo and screen so that they can keep running in the background
// also see EXAMPLE COMMAND

#include <stdlib.h>
#include <nfc/nfc.h>
#include <stdio.h> 
#include <time.h>
#include <string.h>

#include <curl/curl.h>

#include "lpd8806led.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

// fd for leds 
int fd;
// buffer for led colors
lpd8806_buffer buf;
// number of leds
const int leds = 12;

// helper for hex printing of bytes
void print_hex(const uint8_t *pbtData, const size_t szBytes) 
{
  size_t  szPos;
  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

// helper to flash some leds. Index is the start index of the leds that should flash
void flash_leds(int index){

  // iterate all leds
  int i;
  for( i=0 ; i<leds ; i++){
    // with 4 being the amount of leds for one sign
    if( i >= index && i < index + 4){

      // the dimmed leds because of tag scan based on index
      write_gamma_color(&buf.pixels[i],0,0,0);
      write_gamma_color(&buf.pixels[i],0,0,0);
      write_gamma_color(&buf.pixels[i],0,0,0);
      write_gamma_color(&buf.pixels[i],0,0,0);

    } else if ( i < 4) {

      // red leds
      write_gamma_color(&buf.pixels[i],150,0,0);
      write_gamma_color(&buf.pixels[i],150,0,0);
      write_gamma_color(&buf.pixels[i],150,0,0);
      write_gamma_color(&buf.pixels[i],150,0,0);

    } else if ( i < 8 ){

      // blue leds
      write_gamma_color(&buf.pixels[i],0,0,150);
      write_gamma_color(&buf.pixels[i],0,0,150);
      write_gamma_color(&buf.pixels[i],0,0,150);
      write_gamma_color(&buf.pixels[i],0,0,150);

    } else if (i < 12){

      // green leds
      write_gamma_color(&buf.pixels[i],0,150,0);
      write_gamma_color(&buf.pixels[i],0,150,0);
      write_gamma_color(&buf.pixels[i],0,150,0);
      write_gamma_color(&buf.pixels[i],0,150,0);


    }
  }

  // flush thins information
  send_buffer(fd,&buf);
}

// helper to init all leds
void init_leds(){
// init leds
  //int fd;              /* SPI device file descriptor */
  //const int leds = 12; /* 50 LEDs in the strand */
  //lpd8806_buffer buf;      /* Memory buffer for pixel values */
  //int count;           /* Count of iterations (up to 3) */
  //int i;               /* Counting Integer */
  set_gamma(2.5,2.5,2.5);
  /* Open SPI device */
  fd = open("/dev/spidev0.0",O_WRONLY);
  if(fd<0) {
      /* Open failed */
    fprintf(stderr, "Error: SPI device open failed.\n");
    exit(1);
  }

  /* Initialize SPI bus for lpd8806 pixels */
  if(spi_init(fd)<0) {
      /* Initialization failed */
    fprintf(stderr, "Unable to initialize SPI bus.\n");
    exit(1);
  }

  /* Allocate memory for the pixel buffer and initialize it */
  if(lpd8806_init(&buf,leds)<0) {
      /* Memory allocation failed */
    fprintf(stderr, "Insufficient memory for pixel buffer.\n");
    exit(1);
  }

  // SET LEDS HERE, update to do on interaction

  flash_leds(12);

}

// main entry point
int main(int argc, const char *argv[])
{

  // check args by count
  //if( sizeof(argv) != 6+1 ){
  //  printf("Check usage, needs 6 arguments, counted [%d]", sizeof(argv));
  //  exit(1);
  //}

  init_leds();

  nfc_device *pnd;
  nfc_target nt;

  // Allocate only a pointer to nfc_context
  nfc_context *context;

  // Initialize libnfc and set the nfc_context
  nfc_init(&context);
  if (context == NULL) {
    printf("Unable to init libnfc (malloc)\n");
    exit(EXIT_FAILURE);
  }

  // Display libnfc version
  const char *acLibnfcVersion = nfc_version();
  (void)argc;
  printf("%s uses libnfc %s\n", argv[0], acLibnfcVersion);

  // Open, using the first available NFC device which can be in order of selection:
  //   - default device specified using environment variable or
  //   - first specified device in libnfc.conf (/etc/nfc) or
  //   - first specified device in device-configuration directory (/etc/nfc/devices.d) or
  //   - first auto-detected (if feature is not disabled in libnfc.conf) device
  pnd = nfc_open(context, argv[1]);

  if (pnd == NULL) {
    printf("ERROR: %s\n", "Unable to open NFC device.");
    exit(EXIT_FAILURE);
  }
  // Set opened NFC device to initiator mode
  if (nfc_initiator_init(pnd) < 0) {
    nfc_perror(pnd, "nfc_initiator_init");
    exit(EXIT_FAILURE);
  }

  const char *deviceId = nfc_device_get_connstring(pnd);
  printf("NFC reader: %s opened\n", nfc_device_get_name(pnd));
  printf("NFC reader connstring: %s \n", deviceId);

  // Poll for a ISO14443A (MIFARE) tag
  const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
  };

  // get the index of the leds to flickr
  int index;
  index = atoi(argv[4]);

  const char* url;
  url = argv[3];
  char body[50];

  while(1){


    // always make sure leds are just on properly on init of loop
    // by giving an out of range index all leds will init
    flash_leds(12);

    if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0) {
      printf("The following (NFC) ISO14443A tag was found:\n");
      printf("    ATQA (SENS_RES): ");
      print_hex(nt.nti.nai.abtAtqa, 2);
      printf("       UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
      print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
      printf("      SAK (SEL_RES): ");
      print_hex(&nt.nti.nai.btSak, 1);
      if (nt.nti.nai.szAtsLen) {
        printf("          ATS (ATR): ");
        print_hex(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
      }

      // convert the tag into human readable format to work with
      char usertagid[20]="";
      size_t szPos;
      for(szPos=0; szPos<nt.nti.nai.szUidLen;szPos++){
       sprintf(usertagid + strlen(usertagid), "%02x", nt.nti.nai.abtUid[szPos]);
     } 

      // logging
     printf("tag found w/ id \"%s\"\n", usertagid); 
     printf("writing to file \"%s\"", argv[2]);

    flash_leds(index);
    usleep(60*1000);
    flash_leds(12);
    usleep(60*1000);
    flash_leds(index);
    usleep(60*1000);
    flash_leds(12);


      // get time
     //struct timeval tv;
     //gettimeofday(&tv,NULL);
      //tv.tv_sec // seconds
      //tv.tv_usec // microseconds
    int ctime;
    ctime = ((unsigned)time(NULL))*1000; // convert to ms

      // write that tag to the file arg
      // TODO write all params so we can use this as a backup in case of no network
     FILE *file; 
      file = fopen(argv[2],"a+"); /* apend file (add text to 
      a file or create a file if it does not exist.*/ 
      fprintf(file,"%d;%s\n",ctime, usertagid); /*writes*/ 
      fclose(file); /*done!*/ 

        CURL *curl;
        CURLcode res; 
  /* get a curl handle */ 
        curl = curl_easy_init();
        if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
       curl_easy_setopt(curl, CURLOPT_URL, url);
       http://stackoverflow.com/questions/11444583/command-line-curl-timeout-parameter
       curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);
    /* Now specify the POST data */ 

    // prepare the body
    // timestamp and other fields to be added here instead of this sample data
    sprintf( body, "timestamp=%d&tagId=%s&%s", ctime, usertagid, argv[5]);
       curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

    // logging posting to 
    printf("posting to url [%s] with body [%s]", url, body);

    /* Perform the request, res will get the return code */ 
       res = curl_easy_perform(curl);
    /* Check for errors */ 
       if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
          curl_easy_strerror(res));

    /* always cleanup */ 
      curl_easy_cleanup(curl);
    }
    
    } // end of found target if

    // add some wait time here, sleep expects seconds
    // check usleep, should work with microseconds instead
    sleep(1);

  }// end of loop

  // Close NFC device
  nfc_close(pnd);
  // Release the context
  nfc_exit(context);
  exit(EXIT_SUCCESS);
}


