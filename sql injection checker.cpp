#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
  char domain[256];
  printf("Masukkan domain mas: ");
  fgets(domain, sizeof(domain), stdin);

  // Buat URL untuk melakukan query
  char url[512];
  sprintf(url, "https://api.shodan.io/v2/domains/%s/search?query=sqli", domain);

  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    printf("yah GAGAL mas : %s\n", curl_easy_strerror(res));
    return 1;
  }


  char *resp = curl_easy_getinfo(curl, CURLINFO_CONTENT_STRING, NULL);
  if (resp == NULL) {
    printf("Respons HTTP kosong mas.\n");
    return 1;
  }

  JSON_Value *value = json_parse(resp);
  if (value == NULL) {
    printf("Gagal respons mas.\n");
    return 1;
  }

  JSON_Array *results = json_object_get_array(value, "matches");
  if (results == NULL) {
    printf("hemmm ngga ada apa-apa");
    return 0;
  }

  for (int i = 0; i < json_array_get_count(results); i++) {
    JSON_Object *result = json_array_get_object(results, i);

    JSON_Array *parameters = json_object_get_array(result, "parameters");
    for (int j = 0; j < json_array_get_count(parameters); j++) {
      JSON_Object *parameter = json_array_get_object(parameters, j);

      if (json_object_get_boolean(parameter, "vulnerable")) {
        printf("Cepat Perbaiki Domain %s rentan SQL injection bro.!!!.\n", domain);
        return 1;
      }
    }
  }

  printf("Domain %s aman dari SQL injection mas.\n", domain);
  return 0;
}
