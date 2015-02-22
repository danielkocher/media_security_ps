#include "quant.h"

#ifndef METHOD_H_
#define METHOD_H_

typedef int method_func(struct hist *);

extern method_func *quantify_method[];

#endif /* METHOD_H_ */

