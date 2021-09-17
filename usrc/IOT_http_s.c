#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <endian.h>
#include "log.h"
/*about libcurl API,you can refer the weisite https://curl.haxx.se/libcurl/c/*/

size_t write_callback(char *ptr,size_t size,size_t nmemb,void *userdata)
{
  size_t return_size = size * nmemb;
	//char *fp = (char *)userdata;
  memcpy((char *)userdata,ptr,return_size);
	//size_t return_size = fwrite(ptr,size,nmemb,fp);
	return return_size;
}

int http_get(char *url, char *rename)
{
  CURL *curl;
  CURLcode res;
  FILE *fp;
  
 if (url == NULL || rename == NULL)
 {
	printf("Param is NULL\r\n");
	return 0;
 }

  curl_global_init(CURL_GLOBAL_DEFAULT);
  
  char *version = curl_version();
  zlg_debug("libcur Version = %s\r\n", version);
  curl = curl_easy_init();
  if(curl) {
	/*set url,this must set*/
    curl_easy_setopt(curl, CURLOPT_URL, url);
	/*set callback, if not set ,the result will output to stdout*/
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
	//fp = fopen(rename, "w+");
	/*fp point the fourth param of write_callback*/
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, rename);
    /*the function is sync,will block until return*/
    res = curl_easy_perform(curl);

    if(CURLE_OK == res) {
      char *ct = NULL;      
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);	  
      if((CURLE_OK == res) && ct)
        zlg_debug("Content-Type: %s\r\n", ct);
	
	  double downLength = 0;
	  res = curl_easy_getinfo(curl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&downLength);
	  if((CURLE_OK==res) && downLength)
		zlg_debug("FileLength=%.0f\r\n", downLength);

    }
	else{
      zlg_debug("curl_easy_perform() failed: %s\r\n",
              curl_easy_strerror(res));
    }
	//fclose(fp);    
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return res;
}


int https_get(char *url, char *rename, int ca_flag, char*capath, char *cainfo)
{
  CURL *curl;
  CURLcode res;
  FILE *fp;

  if (url == NULL || rename == NULL)
  {
	  printf("Param is NULL\n");
	  return 0;
  }

  if ((ca_flag == 1) && (capath == NULL || cainfo == NULL))
  {
	  printf("Param is NULL\n");
	  return 0;
  }
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
	fp = fopen(rename, "w+");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

//#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    if (ca_flag == 0) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    else if (ca_flag == 1) {
        curl_easy_setopt(curl, CURLOPT_CAPATH , capath);
        curl_easy_setopt(curl, CURLOPT_CAINFO , cainfo);
    }

//#endif

//#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//#endif

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(CURLE_OK == res) {
      char *ct = NULL;      
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);	  
      if((CURLE_OK == res) && ct)
        printf("Content-Type: %s\n", ct);
	
	  double downLength = 0;
	  res = curl_easy_getinfo(curl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&downLength);
	  if((CURLE_OK==res) && downLength)
		printf("FileLength=%.0f\n", downLength);

    }
	else{
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
	fclose(fp);    
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  return 0;
}

int Http_CheckCheck(void)
{    
    int sockfd;
    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    servaddr.sin_addr.s_addr = inet_host(HTTPHOST);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        zlg_debug("socket error\r\n");
        return (-1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) != 0) {  
        zlg_debug("connect error\r\n");
        close(sockfd);
        return (-1);
    }
    close(sockfd);
    return 0;
}

_IOT_http_s IOT_http_s = 
{
  http_get,
  Http_CheckCheck
};
