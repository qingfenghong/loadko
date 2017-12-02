#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "inirw.h"
#include "strfunc.h"

#define FILEPATH  "/mnt/mtd/rom.ini"
#define JSONPATH  "/mnt/mtd/platform/"
#define INSOMOD_PREFIX "insmod  /temp/ko/extdrv"

#ifndef DBG_PRT
#define DBG_PRT printf
#endif

int load_ko(int device_type)
{
	DBG_PRT("startup %s\n", __FUNCTION__);
	int ret = -1;
	char json_pathname[64] = "";
	FILE *fp = NULL;
	int file_szie = 0;
	char *file_buf = NULL;
	cJSON *root_json = NULL;
	cJSON *driver_json = NULL;
	cJSON *sub_json = NULL;
	cJSON *tmp_json = NULL;
	int driver_cnt = 0;
	int i = 0;
	char cmd_buf[128] = {};
	char *sleep_ptr = NULL;


	snprintf(json_pathname, sizeof(json_pathname), "%s/%d.json", JSONPATH, device_type);
	fp = fopen(json_pathname, "r");
	if (!fp)
	{
		DBG_PRT("fopen %s is failed\n", json_pathname);
		goto loadko_exit;
	}
	fseek(fp, 0, SEEK_END);
	file_szie = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (!file_szie)
	{
		DBG_PRT("%s is empty\n", json_pathname);
		goto loadko_exit;
	}
	file_buf = (char *)malloc(file_szie);
	if (!file_buf)
	{
		DBG_PRT("malloc is failed\n");
		goto loadko_exit;
	}
	fread(file_buf, file_szie, 1, fp);
	fclose(fp);
	fp = NULL;
	root_json = cJSON_Parse(file_buf);
	if (!root_json)
	{
		DBG_PRT("parse json is failed form %s\n", json_pathname);
		goto loadko_exit;
	}
	driver_json = cJSON_GetObjectItem(root_json, "Driver");
	if (!driver_json)
	{
		DBG_PRT("get driver object was failed from %s\n", json_pathname);
		goto loadko_exit;
	}
	driver_cnt = cJSON_GetArraySize(driver_json);
	for (i = 0;i < driver_cnt;i++)
	{
		sub_json = cJSON_GetArrayItem(driver_json, i);
		tmp_json = cJSON_GetObjectItem(sub_json, "name");
		if (tmp_json)
		{
			memset(cmd_buf, 0, sizeof(cmd_buf));
			snprintf(cmd_buf, sizeof(cmd_buf), "%s/%s", INSOMOD_PREFIX, tmp_json->valuestring);
			if ((sleep_ptr = strstr(cmd_buf, "sleep")))
			{
				DBG_PRT("start sleep %d second!\n", atoi(sleep_ptr + strlen("sleep")));
				usleep(1000 * 1000 * atoi(sleep_ptr + strlen("sleep")));
				continue;
			}
			
			system(cmd_buf);
		}
	}
	ret = 0;

loadko_exit:
	if(fp)
		fclose(fp);
	if(file_buf)
		free(file_buf);
	if(root_json)
		cJSON_Delete(root_json);
	fp = NULL;
	file_buf = NULL;
	root_json = NULL;

	return ret;
}


int main (int argc, const char * argv[])
{
	int device_type;
	
	char *sect;
	char *key;
	char value[256];
	int intval;
	
	if(0 == iniFileLoad(FILEPATH))
	{
		printf("load ko param is failed from %s\n",FILEPATH);
		return -1;
	}
	
	sect = "DESC";
	key = "DEVICE_TYPE";
	iniGetString(sect, key, value, sizeof(value), "notfound!");
	DBG_PRT("[%s] %s = %s\n", sect, key, value);

	StrToNumber(value, &device_type);
	DBG_PRT("%d\n", device_type);

	load_ko(device_type);
	
	return 0;
}
