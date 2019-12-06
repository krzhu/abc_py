/**
 * @file AbcIncl.h
 * @brief The essential components need to be included in this frameowrk. Codes copy from original abc
 * @author Keren Zhu
 * @date 11/15/2019
 */

#ifndef ABC_PY_ABC_INCL_H_
#define ABC_PY_ABC_INCL_H_


#if defined(ABC_NAMESPACE)
namespace ABC_NAMESPACE
{
#elif defined(__cplusplus)
extern "C"
{
#endif

// procedures to start and stop the ABC framework
// (should be called before and after the ABC procedures are called)
void   Abc_Start();
void   Abc_Stop();

// procedures to get the ABC framework and execute commands in it
typedef struct Abc_Frame_t_ Abc_Frame_t;

Abc_Frame_t * Abc_FrameGetGlobalFrame();
int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );


#if defined(ABC_NAMESPACE)
}
using namespace ABC_NAMESPACE;
#elif defined(__cplusplus)
}
#endif

#endif //ABC_PY_ABC_INCL_H_
