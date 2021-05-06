/*************************************************************************
	> File Name: parse_conf.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Oct 2020 03:29:04 PM CST
 ************************************************************************/

#include "head.h"

//配置文件信息获取
int get_string(const char *file, char *buff, size_t size) {
    FILE *fp = NULL;
    if ((fp = fopen(file, "r")) == NULL) return -1;
    //用fread()读，因为它是按字节读的
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (len > size) {
        fprintf(stderr, "Buffer is less than the len of file!\n");
        return -1;
    }
    fread(buff, len, 1, fp);
    printf(GREEN "%s\n" NONE, buff);
    fclose(fp);
    return len;
}

int parse_conf(char *string) {
    const cJSON *master = NULL;
    const cJSON *slave = NULL;
    cJSON *conf = NULL;

    int status = 0;

    if ((conf = cJSON_Parse(string)) == NULL) {
        const char *err_ptr = cJSON_GetErrorPtr();
        if (err_ptr != NULL) {
            fprintf(stderr, RED "Error Before : %s\n" NONE, err_ptr);
            status = -1;
            goto end;
        }
    }

    master = cJSON_GetObjectItemCaseSensitive(conf, "Master");
    slave = cJSON_GetObjectItemCaseSensitive(conf, "Slave");
    
    cJSON *master_port = cJSON_GetObjectItemCaseSensitive(master, "MasterPort");
    cJSON *slave_port = cJSON_GetObjectItemCaseSensitive(slave, "SlavePort");

    printf("MasterPort = %d, SlavePort = %d\n", master_port->valueint, slave_port->valueint);

    end:
    cJSON_Delete(conf);
    return status;
}

//定义配置获取的接口
cJSON *get_cjson(const char *file) {
    FILE *fp;
    fp = fopen(file, "r");
    char buff[512] = {0};
    char tmp[512] = {0};
    int j = 0;
    while (fgets(tmp, sizeof(tmp), fp) != NULL) {
        j += sprintf(buff + j, "%s", tmp);
    }
    cJSON *conf = NULL;
    if ((conf = cJSON_Parse(buff)) == NULL) {
        const char *err_ptr = cJSON_GetErrorPtr();
        if (err_ptr != NULL) {
            fprintf(stderr, RED "Error Before: %s\n" NONE, err_ptr);
        }
    }    
    return conf;
}

int get_json_valueint(cJSON *conf, const char *who, const char *config) {
    const cJSON *host = NULL;
    cJSON *ret = NULL;
    host = cJSON_GetObjectItemCaseSensitive(conf, who);
    ret = cJSON_GetObjectItemCaseSensitive(host, config);
    return ret->valueint;
}

char *get_json_valuestring(cJSON *conf, const char *who, const char *config) {
    const cJSON *host = NULL;
    cJSON *ret = NULL;
    host = cJSON_GetObjectItemCaseSensitive(conf, who);
    ret = cJSON_GetObjectItemCaseSensitive(host, config);
    return ret->valuestring;
}

int get_json__array_valueint(cJSON *conf, const char *who, const char *config, const char *shell) {
    const cJSON *host = NULL;
    const cJSON *confcjson = NULL;
    cJSON *ret = NULL;
    host = cJSON_GetObjectItemCaseSensitive(conf, who);
    confcjson = cJSON_GetObjectItemCaseSensitive(host, config);
    ret = cJSON_GetObjectItemCaseSensitive(confcjson, shell);
    return ret->valueint;
}
