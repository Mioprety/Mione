#include <stdio.h>
#include "verson.h"

int prerr(int line, char* msg, int errCode) {
	printf("���~���ض}�Y��ơG%d\n���~���U�����G%s \n", line, msg);
	printf("	Mione�����G%s\n", mione_verson);
	printf("	���~�N�X�G'%d'\n", errCode);
	return 1;
}