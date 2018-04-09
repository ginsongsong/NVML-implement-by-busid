#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

const char * convertToComputeModeString(nvmlComputeMode_t mode)
{
    switch (mode)
    {
        case NVML_COMPUTEMODE_DEFAULT:
            return "Default";
        case NVML_COMPUTEMODE_EXCLUSIVE_THREAD:
            return "Exclusive_Thread";
        case NVML_COMPUTEMODE_PROHIBITED:
            return "Prohibited";
        case NVML_COMPUTEMODE_EXCLUSIVE_PROCESS:
            return "Exclusive Process";
        default:
            return "Unknown";
    }
}

int main()
{
    nvmlReturn_t result;
    unsigned int device_count, i;

    // First initialize NVML library
    result = nvmlInit();
    if (NVML_SUCCESS != result)
    { 
        printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));

        printf("Press ENTER to continue...\n");
        getchar();
        return 1;
    }

    result = nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != result)
    { 
     //   printf("Failed to query device count: %s\n", nvmlErrorString(result));
    }
   // printf("Found %d device%s\n\n", device_count, device_count != 1 ? "s" : "");


    char * SH_SL=(char*)malloc(sizeof(char)*60);
    const char *BID[8] = { "00000000:1B:00.0", "00000000:1C:00.0","00000000:1D:00.0","00000000:1E:00.0","00000000:DB:00.0"," 00000000:DC:00.0","00000000:DD:00.0","00000000:DE:00.0" };
    	
    //printf("Listing devices:\n");
	
	strcat(SH_SL,"ipmitool raw  0x30 0xc5 ");

	for (i = 0; i < 8 ; i++) // for eigtht devices
	{
		nvmlDevice_t device;
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		nvmlPciInfo_t pci;
		nvmlComputeMode_t compute_mode;

		//result = nvmlDeviceGetHandleByIndex(i, &device);
		result = nvmlDeviceGetHandleByPciBusId( BID[i] , &device );
		if (NVML_SUCCESS != result)
		{ 
			//printf("Failed to get handle for device %i: %s\n", i, nvmlErrorString(result));
			
			strcat(SH_SL,"0xFF ");
			strcat(SH_SL,"0xFF ");
			continue;
			
		}
		
		unsigned int SH_t=0;
		unsigned int SL_t=0;
		result= nvmlDeviceGetTemperatureThreshold(device,0,&SH_t);
		result= nvmlDeviceGetTemperatureThreshold(device,1,&SL_t);

		char res_H[2];
		if (SH_t <= 0xFF)
		{
			res_H[0] = TO_HEX(((SH_t & 0xF0) >> 4));
    			res_H[1] = TO_HEX((SH_t & 0x0F));
    			res_H[2] = ' ';
		}
		char res_L[2];
		if (SL_t <= 0xFF)
		{
			res_L[0] = TO_HEX(((SL_t & 0xF0) >> 4));
    			res_L[1] = TO_HEX((SL_t & 0x0F));
    			res_L[2] = ' ';
		}
		
		strcat(SH_SL,"0x");
		strcat(SH_SL,res_H);
		strcat(SH_SL,"0x");
    		strcat(SH_SL,res_L);

	}
//	printf("%s\n",SH_SL);
	system(SH_SL);
	
	char* C_TEMP=malloc(sizeof(char)*60);
	memset(C_TEMP, 0, sizeof(char)*60);
	unsigned int C_t;

	for(;;)
	{   
		memset(C_TEMP, 0, sizeof(char)*60);
    		
		strcat(C_TEMP,"ipmitool raw  0x30 0xc6 ");
		for (i = 0; i < 8; i++)
		{
			nvmlDevice_t device;
			char name[NVML_DEVICE_NAME_BUFFER_SIZE];
			nvmlPciInfo_t pci;
			nvmlComputeMode_t compute_mode;

			result = nvmlDeviceGetHandleByPciBusId( BID[i] , &device );
			if (NVML_SUCCESS != result)
			{ 
			//	printf("Failed to get handle for device %i: %s\n", i, nvmlErrorString(result));
				strcat(C_TEMP,"0xFF ");
				strcat(C_TEMP,"0xFF ");
				continue;
			}
			result= nvmlDeviceGetTemperature(device,0, &C_t);
			char res_T[2];
			if (C_t <= 0xFF)
			{	
				res_T[0] = TO_HEX(((C_t & 0xF0) >> 4));
    				res_T[1] = TO_HEX((C_t & 0x0F));
    				res_T[2] = ' ';
			}
			strcat(C_TEMP,"0x");
			strcat(C_TEMP,res_T);
		}
		printf("%s\n",C_TEMP);
		system(C_TEMP);
		sleep(2);
	}//for end

    result = nvmlShutdown();
    if (NVML_SUCCESS != result)
        printf("Failed to shutdown NVML: %s\n", nvmlErrorString(result));

    free(SH_SL);
    free(C_TEMP);
    return 0;
}


