#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "jsmn.h"
#include <microhttpd.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"
#define JSON_TOKEN_MAX_LENGTH 1000

struct connection_info {
  char *post_data;
  size_t post_data_size;
};

int get_token_string(const char *json, jsmntok_t *tok, char *output, size_t size) {
  if (tok->end < 0 || tok->start < 0) {
    printf("error: token bounds are negative, usually means error parsing. tok->start = %d, tok->end = %d\n", tok->start, tok->end);
    return -1;
  }

  if (tok->end < tok->start) {
    printf("error: token ends before it starts\n");
    return -2;
  }
  int length = tok->end - tok->start;
  //printf("length: %d\n", length);

  if (length >= size) {
    length = size - 1;
  }

  if (output != NULL) {
    strncpy(output, json + tok->start, length);
    output[length] = '\0';
  }
  return length;
}
void print_token(const char *json, jsmntok_t *tok) {
  int length = tok->end - tok->start;
  char *token_str = malloc(length + 1);
  int chars_read = get_token_string(json, tok, token_str, length + 1);
  if (chars_read < 0) {
    printf("Error parsing, can't print\n");
  } else {
    printf("%s\n", token_str);
  }
  free(token_str);
  token_str = NULL;
}

int get_num_tokens_in_object(const char *json, int token_index, jsmntok_t *tokens, size_t tokens_size);


int get_num_tokens_in_array(const char *json, int token_index, jsmntok_t *tokens, size_t tokens_size) {
  if (token_index >= tokens_size) {
    printf("error: passed an out-of-bounds value to get_num_tokens_in_array. Passed index: %d, size:%zu\n", token_index, tokens_size);
    return -1;
  }
  jsmntok_t *array_token = &tokens[token_index];
  if (array_token->type != JSMN_ARRAY) {
    printf("error: sent a non-array type into get_num_tokens_in_array.. Type = %d\n", array_token->type);
    return -2;
  }
  int first_element_index = token_index + 1;

  int count = 0;
  for (int i = 0; i < array_token->size; i++) {
    int cur_element_index = first_element_index + i;
    if (cur_element_index >= tokens_size) {
      printf("error: array element is out of bounds of tokens. Unknown how this happened\n");
      return -3;
    }
    jsmntok_t *cur_element_token = &tokens[cur_element_index];
    if (cur_element_token->type == JSMN_OBJECT) {
      count += (1 + get_num_tokens_in_object(json, cur_element_index, tokens, tokens_size));
    } else if (cur_element_token->type == JSMN_ARRAY) {
      count += (1 + get_num_tokens_in_array(json, cur_element_index, tokens, tokens_size));
    } else {
      count += 1;
    }
  }
  return count;
      
}

int get_token_length(jsmntok_t *tok) {
  return tok->end - tok->start;
}

char *malloc_get_token_string(char *json, jsmntok_t *tok) {
  int token_str_len = get_token_length(tok);
  char *token_str = malloc(token_str_len + 1);
  int num_chars_read = get_token_string(json, tok, token_str, token_str_len + 1);
  if (num_chars_read < 0) {
    printf("error getting token string\n");
    free(token_str);
    return NULL;
  }
  return token_str;
}

int token_matches_target_key(char *json, jsmntok_t *key_tok, char *key_target) {
  char *key_str = malloc_get_token_string(json, key_tok);
  if (!key_str) {
    printf("error getting key token string\n");
    return 0;
  }
  int ret_val = (0 == strcmp(key_str, key_target));
  free(key_str);
  key_str = NULL;
  return ret_val;
}

void copy_to_output(char *output, char *source, int size) {
  int num_chars_to_copy = strlen(source);
  if (num_chars_to_copy >= size) {
    num_chars_to_copy = size - 1;
  }
  strncpy(output, source, num_chars_to_copy);
  output[num_chars_to_copy] = '\0';
}

int get_json_key_value(char *key, char *json, char *output, size_t output_size) {
  jsmn_parser parser;
  jsmn_init(&parser);
  int num_tokens = jsmn_parse(&parser, json, strlen(json), NULL, 0);
  if (num_tokens < 0) {
    printf("error parsing json\n");
    return -2;
  }
  jsmn_init(&parser);
  jsmntok_t *tokens = malloc(num_tokens * sizeof(jsmntok_t));
  int ret = jsmn_parse(&parser, json, strlen(json), tokens, num_tokens);
  if (ret < 0) {
    printf("error parsing json\n");
    return -2;
  }
  int i = 1;
  while (i < num_tokens) {
    jsmntok_t *key_token = &tokens[i];
    if(token_matches_target_key(json, key_token, key)) {
      char *value_str = malloc_get_token_string(json, &tokens[i + 1]);
      if (!value_str) {
        printf("error getting value string\n");
      }
      if (output != NULL) {
        copy_to_output(output, value_str, output_size);
      }
      int value_str_len = strlen(value_str);
      free(value_str);
      value_str = NULL;
      return value_str_len;
    }
    printf("key: ");
    print_token(json, &tokens[i]);
    i++;
    jsmntok_t *value_ptr = &tokens[i];
    printf("Value: ");
    print_token(json, value_ptr);
    if (value_ptr->type == JSMN_ARRAY) {
      int num_tokens_in_array = get_num_tokens_in_array(json, i, tokens, num_tokens);
      printf("num_tokens_in_array = %d\n", num_tokens_in_array);
      i += (1 + num_tokens_in_array);
    }
    else if (value_ptr->type == JSMN_OBJECT) {
      int num_tokens_in_object = get_num_tokens_in_object(json, i, tokens, num_tokens);
      i += (1 + num_tokens_in_object);
    } else {
      i += 1;
    }
  }
  return -1; // did not find value
}

int get_num_tokens_in_object(const char *json, int token_index, jsmntok_t *tokens, size_t tokens_size) {
  if (token_index >= tokens_size) {
    printf("error: passed an out-of-bounds value to get_num_tokens_in_object. Passed index: %d, size:%zu\n", token_index, tokens_size);
    return -1;
  }
  jsmntok_t *cur_token = &tokens[token_index];
  if (cur_token->type != JSMN_OBJECT) {
    printf("error: sent a non-object type into num_tokens_in_object.. Type = %d\n", cur_token->type);
    return -2;
  }
  int first_key_string_index = token_index + 1;
  int count = 0;
  for (int i = 0; i < cur_token->size; i++) {
    int cur_key_string_index = first_key_string_index + i;
    if (cur_key_string_index >= tokens_size) {
      printf("error: key string index is out of bounds. Unknown how this could happen.\n");
      return -3;
    }
    if (tokens[cur_key_string_index].type != JSMN_STRING) {
      printf("error: object key must be string.. Type = %d\n", tokens[cur_key_string_index].type);
      return -4;
    }
    int value_index = cur_key_string_index + 1;
    if (value_index >= tokens_size) {
      printf("error: object value token index is out of bounds\n");
      return -5;
    }
    count += 1; // for key
    jsmntok_t *value = &tokens[value_index];
    if (value->type == JSMN_ARRAY) {
      // `(1 + ...)` because whole array is a token too
      count += (1 + get_num_tokens_in_array(json, value_index, tokens, tokens_size));
    } else if (value->type == JSMN_OBJECT) {
      // `(1 + ...)` because whole object is a token too
      count += (1 + get_num_tokens_in_object(json, value_index, tokens, tokens_size));
    } else {
      count += 1;
    }
  }
  return count;
}

void free_memory_callback(void *data) {
  free(data);
}

static enum MHD_Result handle_post(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** con_cls) {
  struct connection_info *con_info;

  if (0 != strcmp(method, "POST"))
    return MHD_NO; /* unexpected method */

  if (*con_cls == NULL) {
    con_info = malloc(sizeof(struct connection_info));
    con_info->post_data = NULL;
    con_info->post_data_size = 0;
    *con_cls = con_info;
    return MHD_YES;
  }

  con_info = *con_cls;

  if (*upload_data_size) {
    con_info->post_data = realloc(con_info->post_data,
                                  con_info->post_data_size + *upload_data_size + 1);
    memcpy(con_info->post_data + con_info->post_data_size,
           upload_data, *upload_data_size);
    con_info->post_data_size += *upload_data_size;
    con_info->post_data[con_info->post_data_size] = '\0';
    *upload_data_size = 0;
    return MHD_YES;
  }
  else {
    printf("received POST data: %s\n", con_info->post_data);

    struct MHD_Response *response;

    int value_length = get_json_key_value("name", con_info->post_data, NULL, 0);
    printf("value_length = %d\n", value_length);

    char *response_str = NULL;
    if (value_length == -1) {
      printf("did not find key 'name'\n");
      char *tmp_response = "{\"status\":\"success\"}";
      int response_len = strlen(tmp_response);
      response_str = malloc(response_len + 1);
      snprintf(response_str, response_len + 1, "%s", response_str);
    } else if (value_length == -2) {
      printf("error parsing json\n");
      char *tmp_response = "{\"status\":\"fail\"}";
      int response_len = strlen(tmp_response);
      response_str = malloc(response_len + 1);
      snprintf(response_str, response_len + 1, "%s", response_str);
    } else {
      char *value_str = malloc(value_length + 1);
      get_json_key_value("name", con_info->post_data, value_str, value_length + 1);

      char *message_before_insert = "{\"status\":\"success\",\"name\":\"";
      char *message_after_insert = "\"}";
      int full_message_len = strlen(message_before_insert) + value_length + strlen(message_after_insert);
      response_str = malloc(full_message_len + 1);
      int result = snprintf(response_str, full_message_len + 1, "%s%s%s", message_before_insert, value_str, message_after_insert);
      if (result >= full_message_len + 1) {
        printf("error: output json was truncated, malloc'd %d bytes, but required %d bytes\n", full_message_len + 1, result + 1);
      }

      free(value_str);
      value_str = NULL;
    }
    printf("response_str = %s\n", response_str);
    response = MHD_create_response_from_buffer_with_free_callback(strlen(response_str), response_str, &free_memory_callback);

    //char *message = "POST received, had errors";
    //response = MHD_create_response_from_buffer(strlen(message), message, MHD_RESPMEM_PERSISTENT);

    int ret = MHD_queue_response(connection,
                   MHD_HTTP_OK,
                   response);
    MHD_destroy_response(response);
    free(con_info->post_data);
    free(con_info);
    *con_cls = NULL;

    return ret;
  }
}

static enum MHD_Result
time_api(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** ptr) {
  static int dummy;
  const char * page = cls;
  struct MHD_Response * response;
  int ret;

  if (0 != strcmp(method, "GET"))
    return MHD_NO;
  if (&dummy != *ptr)
    {
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }
  if (0 != *upload_data_size)
    return MHD_NO; /* upload data in a GET!? */
  *ptr = NULL; /* clear context pointer */

  time_t current_time;
  current_time = time(NULL);
  char time_str[11];
  snprintf(time_str, sizeof(time_str), "%ld", current_time);

  response = MHD_create_response_from_buffer (strlen(time_str),
                                              (void*) time_str,
  					      MHD_RESPMEM_MUST_COPY);
  ret = MHD_queue_response(connection,
			   MHD_HTTP_OK,
			   response);
  MHD_destroy_response(response);
  return ret;
}

enum MHD_Result print_out_key (void *cls, enum MHD_ValueKind kind, 
                       const char *key, const char *value)
{
    printf ("%s: %s\n", key, value);
    return MHD_YES;
}

static enum MHD_Result
print_req_details(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** ptr) {
  printf ("New %s request for %s using version %s\n", method, url, version);
  MHD_get_connection_values (connection, MHD_HEADER_KIND, &print_out_key, NULL);
  return MHD_NO;
}

int main(int argc,
	 char ** argv) {
  struct MHD_Daemon * d;
  if (argc != 2) {
    printf("%s PORT\n",
	   argv[0]);
    return 1;
  }
  printf("JSMN_UNDEFINED = %d\n", JSMN_UNDEFINED);
  printf("JSMN_OBJECT = %d\n", JSMN_OBJECT);
  printf("JSMN_ARRAY = %d\n", JSMN_ARRAY);
  printf("JSMN_STRING = %d\n", JSMN_STRING);
  printf("JSMN_PRIMITIVE = %d\n", JSMN_PRIMITIVE);
  char *port_str = argv[1];
  d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		       atoi(port_str),
		       NULL,
		       NULL,
		       &handle_post,
		       PAGE,
		       MHD_OPTION_END);
  if (d == NULL)
    return 1;
  printf("Server listening on port %s\n", port_str);
  (void) getc (stdin);
  MHD_stop_daemon(d);
  return 0;
}
