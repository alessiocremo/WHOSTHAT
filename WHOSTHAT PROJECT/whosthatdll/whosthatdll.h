#ifndef __WHOSTHATDLL_H__
#define __WHOSTHATDLL_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

void DLL_EXPORT Generate_Random_Array(int* rn_array, int db_size, int nrounds);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C"
{
#endif

int DLL_EXPORT Get_Best_Score(char* filename);

#ifdef __cplusplus
}
#endif


#endif // __WHOSTHATDLL_H__
