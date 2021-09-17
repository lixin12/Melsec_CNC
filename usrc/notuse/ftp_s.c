#include <stdio.h>
#include <curl/curl.h>

/* <DESC>
 * Get a single file from an FTP(S) server.
 * </DESC>
 */
static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  FILE *fp = (FILE *)stream;
  return fwrite(buffer, size, nmemb, fp);
}

int ftp_get(char *url, char *rename, char *usr, char *pwd)
{
  CURL *curl;
  CURLcode res;
  FILE *fp;
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if(curl) {
    /*
     * You better replace the URL with one that works!
     */
    curl_easy_setopt(curl, CURLOPT_URL,
                     url);

	fp = fopen(rename, "w+");
    /* Define our callback to get called when there's data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	

    /* Switch on full protocol/debug output */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	
	/*set user pwd*/
	//if (argc == 5)
	{
		char usrpwd[50] = {0};
		sprintf(usrpwd,"%s:%s", usr,pwd);
		curl_easy_setopt(curl,CURLOPT_USERPWD,usrpwd);
	}
    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if(CURLE_OK != res) {
      /* we failed */
      fprintf(stderr, "curl told us %d\n", res);
    }
  }

  if(fp)
    fclose(fp); /* close the local file */

  curl_global_cleanup();

  return 0;
}


//FTPS
int ftps_get(char *url, char *rename, char *usr, char *pwd)
{
  CURL *curl;
  CURLcode res;
  FILE *fp;
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if(curl) {
    /*
     * You better replace the URL with one that works!
     */
    curl_easy_setopt(curl, CURLOPT_URL,
                     url);

	fp = fopen(rename, "w+");
    /* Define our callback to get called when there's data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	

    /* Switch on full protocol/debug output */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	/*no verify certs*/
	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_NONE);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	
	/*set user pwd*/
	//if (argc == 5)
	{
		char usrpwd[50] = {0};
		sprintf(usrpwd,"%s:%s", usr,pwd);
		curl_easy_setopt(curl,CURLOPT_USERPWD,usrpwd);
	}
    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if(CURLE_OK != res) {
      /* we failed */
      fprintf(stderr, "curl told us %d\n", res);
    }
  }

  if(fp)
    fclose(fp); /* close the local file */

  curl_global_cleanup();

  return 0;
}