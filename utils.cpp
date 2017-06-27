#include "stdafx.h"
#include "utils.h"

#pragma warning(disable: 4996)

void asciitohex(char* str, unsigned int* hex)
{
	unsigned int i = 0;
	unsigned int value = 0;
	char temp[2];

	for (i = 0; i<strlen(str); i++) {

		switch (str[i])
		{

		case 'a':
		case 'A':
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);

			value |= atoi(temp) + 10;
			break;
		case 'b':
		case 'B':
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);

			value |= atoi(temp) + 11;
			break;
		case 'c':
		case 'C':
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);

			value |= atoi(temp) + 12;
			break;
		case 'd':
		case 'D':
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);

			value |= atoi(temp) + 13;
			break;
		case 'e':
		case 'E':
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);

			value |= atoi(temp) + 14;
			break;
		case 'f':
		case 'F':
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);

			value |= atoi(temp) + 15;
			break;

		default:
			value = value << 4;
			memset(temp, 0x0, 2);
			memcpy(temp, &str[i], 1);
			//printf("%x\r\n",atoi(temp));
			value |= atoi(temp);
			break;
		}

	}
	*hex = value;
}

char* CString2char(CString& str)
{
	long len = str.GetLength();
	len = len * 2;
	char* szTemp = new char[len + 1];
	memset(szTemp, 0, len + 1);
	USES_CONVERSION;
	strcpy(szTemp, T2A(str));

	return szTemp;
}

float update_fps(UINT frameCount)
{
	static float fps = 0.0f;
	static float timeElapsed = 0.0f, timeDelta;
	static UINT lastTime = 0, curTime;
	static UINT lastCount = 0;
	static UINT count = 0;

	curTime = GetTickCount();

	//frameCount++;
	count += frameCount - lastCount;
	lastCount = frameCount;

	if (lastTime == 0)
	{
		lastTime = curTime;
		return 0.0f;
	}

	timeDelta = (curTime - lastTime)*0.001f;

	timeElapsed += timeDelta;

	if (timeElapsed >= 1.0f)
	{
		fps = (float)count / timeElapsed;

		count = 0;
		timeElapsed = 0.0f;
	}

	lastTime = curTime;

	return fps;
}
