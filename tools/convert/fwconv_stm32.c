#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLASH_MAIN_APPL			0x8005800
#define FLASH_META_MAGIC		0x801F700
#define FLASH_META_FW_INFO		0x801F780

#define BOOTLOADER_FW_DIR		"BOOTLOADER"
#define MAIN_APPL_FW_DIR		"MAIN_APPL"

#define BOOTLOADER_FW_NAME		"MICOM_STM32_BOOTLOADER.bin"
#define MAIN_APPL_FW_NAME		"MICOM_STM32_MAIN_APPL.bin"

#define DEFAULT_MODEL_NAME		"CR3"

#define SIZE_128KB				(128*1024)

#pragma pack(pop)
typedef struct {
	char info[16];
	char date[16];
	char time[16];
	char ver_char;
	char ver_major;
	char ver_minor;
	char ver_revision;
	char ver_hidden;
	char model_name[32];
} EMT_FW_VER_INFO_T;
#pragma pack(push, 1)

char* _pModelName = NULL;
int   _iSkipModelName = 0;



static int filecopy(const char* psour, const char* pdest, size_t file_size) 
{
	FILE	* fsour, * fdest;
	char	buff[1024];
	size_t	n_size, remain_size = file_size;

	if (!strcmp(psour, pdest))   return       -1; // 원본과 사본 파일이 동일하면 에러

	if (NULL == (fsour = fopen(psour, "rb")))    return -2; // 원본 파일 열기
	if (NULL == (fdest = fopen(pdest, "wb"))) {
		fclose(fsour);
		return -3;
	} // 대상 파일 만들기

	while (0 < (n_size = fread(buff, 1, sizeof(buff), fsour))) {
		if (0 == fwrite(buff, 1, n_size, fdest)) {
			fclose(fsour);
			fclose(fdest);
			unlink(pdest); // 에러난 파일 지우고 종료
			return -4;
		}
		remain_size -= n_size;
	}
	fclose(fsour);

	if (remain_size) // 나머지 0x00으로 채우기
	{
		memset(buff, 0xff, sizeof(buff));
		if (remain_size >= 1024)
		{
			n_size = 1024;
		}

		do
		{
			if (remain_size < 1024)
			{
				n_size = remain_size;
			}

			fwrite(buff, 1, n_size, fdest);

			remain_size -= n_size;

		} while (remain_size > 0);
	}

	fclose(fdest);

	return 0;
}

static int fileappend(const char* psour, const char* pdest)
{
	FILE* fsour, * fdest;
	char	buff[1024];
	size_t	n_size;

	if (!strcmp(psour, pdest))   return       -1; // 원본과 사본 파일이 동일하면 에러

	if (NULL == (fsour = fopen(psour, "rb")))    return -2; // 원본 파일 열기
	if (NULL == (fdest = fopen(pdest, "ab"))) {
		fclose(fsour);
		return -3;
	} // 대상 파일 만들기

	
	fseek(fdest, 0L, SEEK_END);

	while (0 < (n_size = fread(buff, 1, sizeof(buff), fsour))) {
		if (0 == fwrite(buff, 1, n_size, fdest)) {
			fclose(fsour);
			fclose(fdest);
			unlink(pdest); // 에러난 파일 지우고 종료
			return -4;
		}
	}
	fclose(fsour);
	fclose(fdest);

	return 0;
}

static int convert_main_appl_image(char* p_project_dir_path, EMT_FW_VER_INFO_T	*p_fw_ver_info)
{
	FILE* fp;
	int res = -1;
	char file_path[256] = { 0 };
	size_t file_size = 0;
	char read_buffer[128] = { 0 };
	
	if (_iSkipModelName == 0)	sprintf(file_path, "%s%s\\%s_%s", p_project_dir_path, MAIN_APPL_FW_DIR, _pModelName, MAIN_APPL_FW_NAME);
	else						sprintf(file_path, "%s%s\\%s", p_project_dir_path, MAIN_APPL_FW_DIR, MAIN_APPL_FW_NAME);
	fp = fopen(file_path, "rb");
	if (NULL == fp)
	{
		if (_iSkipModelName == 0)	printf("%s_%s fopen fail !!!\r\n", _pModelName, MAIN_APPL_FW_NAME);
		else						printf("%s fopen fail !!!\r\n", MAIN_APPL_FW_NAME);
		return -1;
	}

	res = fseek(fp, 0L, SEEK_END);//end of binary
	if (0 != res)
	{
		if (_iSkipModelName == 0)	printf("%s_%s fseek end fail !!!\r\n", _pModelName, MAIN_APPL_FW_NAME);
		else						printf("%s fseek end fail !!!\r\n", MAIN_APPL_FW_NAME);
		fclose(fp);
		return -2;
	}

	file_size = ftell(fp);//get input-file's size
	if (file_size >= ((FLASH_META_FW_INFO - FLASH_MAIN_APPL) + sizeof(EMT_FW_VER_INFO_T)))
	{
		res = fseek(fp, 0L, SEEK_SET);
		if (res == 0)
		{
			res = fseek(fp, (FLASH_META_FW_INFO - FLASH_MAIN_APPL), SEEK_SET);//set file-pointer on start of binary
			if (res == 0)
			{
				res = fread(read_buffer, 1, sizeof(EMT_FW_VER_INFO_T), fp);
				if (res == sizeof(EMT_FW_VER_INFO_T))
				{
					memcpy(p_fw_ver_info, read_buffer, sizeof(EMT_FW_VER_INFO_T));
				}
			}
		}
		
	}
	fclose(fp);

	int modelname_len = strlen(p_fw_ver_info->model_name);

	if (_iSkipModelName == 0) {
		int modelname_len2 = strlen(_pModelName);

		if (modelname_len != modelname_len2) {
			return -3;
		}

		if (memcmp(p_fw_ver_info->model_name, _pModelName, modelname_len) != 0) {
			return -4;
		}
	}

	if (modelname_len > 0)
	{
		char out_file_path[256] = { 0 };
		sprintf(out_file_path, "%s%s_MCU.bin", p_project_dir_path, p_fw_ver_info->model_name);

		res = filecopy(file_path, out_file_path, file_size);
		if (res == 0)
		{
			printf("Converted FW File : %s_MCU.bin !!!\r\n", p_fw_ver_info->model_name);
		}
	}
	else
	{
		res = -5;
	}

	return res;
}

static int make_dummy_image(const char* pfilepath)
{
	FILE* fp;
	int res = -1, i = 0;
	char *buffer;
	size_t file_size = 0, remain_size = 0;

	fp = fopen(pfilepath, "rb+");
	if (fp == NULL) {
		printf("%s fopen fail !!!\r\n", pfilepath);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (file_size < SIZE_128KB) {
		remain_size = SIZE_128KB - file_size;

		buffer = (char *)malloc(remain_size * sizeof(char));
		if (buffer == NULL) {
			printf("malloc fail\r\n");
			fclose(fp);
			return -2;
		}
		fseek(fp, 0, SEEK_END);

		for (i = 0; i < remain_size; i++) {
			buffer[i] = 0xFF;
		}
		fwrite(buffer, sizeof(char), remain_size, fp);

		free(buffer);
		fclose(fp);
	}

	return 0;
}

static int make_gang_image(char * p_project_dir_path, int main_appl_start_address, EMT_FW_VER_INFO_T* p_fw_ver_info)
{
	FILE* fp;
	int res = -1;
	char file_path[256] = { 0 };
	int file_size = 0;
	char read_buffer[128] = { 0 };
	char out_file_path[256] = { 0 };
	

	if (_iSkipModelName == 0)	sprintf(file_path, "%s%s\\%s_%s", p_project_dir_path, BOOTLOADER_FW_DIR, _pModelName, BOOTLOADER_FW_NAME);
	else						sprintf(file_path, "%s%s\\%s", p_project_dir_path, BOOTLOADER_FW_DIR, BOOTLOADER_FW_NAME);
	fp = fopen(file_path, "rb");
	if (NULL == fp)
	{
		if (_iSkipModelName == 0)	printf("%s_%s fopen fail !!!\r\n", _pModelName, BOOTLOADER_FW_NAME);
		else						printf("%s fopen fail !!!\r\n", BOOTLOADER_FW_NAME);
		return -1;
	}

	res = fseek(fp, 0L, SEEK_END);//end of binary
	if (0 != res)
	{
		if (_iSkipModelName == 0)	printf("%s_%s fseek end fail !!!\r\n", _pModelName, BOOTLOADER_FW_NAME);
		else						printf("%s fseek end fail !!!\r\n", BOOTLOADER_FW_NAME);
		fclose(fp);
		return -2;
	}
	fclose(fp);
	
	sprintf(out_file_path, "%s%s_BOOT_APP_%c%c%c%c.bin", p_project_dir_path, p_fw_ver_info->model_name, p_fw_ver_info->ver_char, p_fw_ver_info->ver_major, p_fw_ver_info->ver_minor, p_fw_ver_info->ver_revision);
	res = filecopy(file_path, out_file_path, main_appl_start_address);

	if (_iSkipModelName == 0)	sprintf(file_path, "%s%s\\%s_%s", p_project_dir_path, MAIN_APPL_FW_DIR, _pModelName, MAIN_APPL_FW_NAME);
	else						sprintf(file_path, "%s%s\\%s", p_project_dir_path, MAIN_APPL_FW_DIR, MAIN_APPL_FW_NAME);
	res = fileappend(file_path, out_file_path);
	if (res == 0)
	{
		printf("Gang Image File : %s_BOOT_APP_%c%c%c%c.bin\r\n", p_fw_ver_info->model_name, p_fw_ver_info->ver_char, p_fw_ver_info->ver_major, p_fw_ver_info->ver_minor, p_fw_ver_info->ver_revision);
	}

	make_dummy_image(out_file_path);
	return res;
}

//arg[0]:exe filename
//arg[1]:input project path
//arg[2]:main app start address => 0x5800(0x8005800)
//arg[3]:model name
int main(int argc, char *argv[])
{
#if 0//debug test
	//argv[1] = "C:\\cygwin-1.7.18\\home\\v57_japan\\sphost_ZDR065\\micom_stm32\\";
	argv[1] = "E:\\project\\bb\\micom_stm32\\";
	argv[2] = "0x5800";
	argv[3] = "CR3";

	printf("debug >> argv[0]=[%s]\n", argv[0]);
	printf("debug >> argv[1]=[%s]\n", argv[1]);
	printf("debug >> argv[2]=[%s]\n", argv[2]);
	printf("debug >> argv[3]=[%s]\n", argv[3]);
#endif

	int	res = -1;
	char* p_project_dir_path = argv[1];
	char* p_main_appl_fw_start_address = argv[2];
	int main_appl_fw_start_address = 0;
	EMT_FW_VER_INFO_T	fw_ver_info;
	printf("* STM32 micom FW convert : build @ 2023.10.10 by kimjj\r\n");

	_pModelName = argv[3];

	if (strlen(_pModelName) < 3) {
		printf("Error : Project Model Name is short. !!!\r\n");
		return -2;
	}

	if (p_project_dir_path == NULL) {
		printf("Error : Project Dir Path is Empty !!!\r\n");
		return -3;
	}
	
	if (p_main_appl_fw_start_address == NULL) {
		printf("Error : Main Appl start address is is Empty !!!\r\n");
		return -4;
	}

	if (memcmp(DEFAULT_MODEL_NAME, _pModelName, strlen(_pModelName)) == 0) {
		_iSkipModelName = 1;
	}

	printf("* project_dir_path : %s\r\n* main_appl_start_address : %s\r\n* model_name : %s\r\n",
			p_project_dir_path, p_main_appl_fw_start_address, _pModelName);

	
	if ((p_main_appl_fw_start_address[0] == '0') && ((p_main_appl_fw_start_address[1] == 'x') || p_main_appl_fw_start_address[1] == 'X'))
	{
		main_appl_fw_start_address = (int)strtol(p_main_appl_fw_start_address, NULL, 0);
	}
	else
	{
		main_appl_fw_start_address = (int)strtol(p_main_appl_fw_start_address, NULL, 16);
	}
	
	memset(&fw_ver_info, 0x00, sizeof(EMT_FW_VER_INFO_T));

	res = convert_main_appl_image(p_project_dir_path, &fw_ver_info);
	if (res == 0)
	{
		printf("Converting FW was SUCCESS !!!\r\n");
	}
	else
	{
		printf("Converting FW was FAILURE [%d]!!!\r\n", res);
		return -5;
	}

	if (main_appl_fw_start_address)
	{
		res = make_gang_image(p_project_dir_path, main_appl_fw_start_address, &fw_ver_info);
		if (res == 0)
		{
			printf("Making Gang Image was SUCCESS !!!\r\n");
		}
		else
		{
			printf("Making Gang Image was Failure !!!\r\n");
		}
	}	

	return res;
}