//
//  rsa.h
//  libmdrsa
//
//  Created by Mert Dümenci on 3/22/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#ifndef rsa_h
#define rsa_h

#include <stdio.h>
#include "prime.h"

void generateKeys(vU1024 *pubE, vU1024 *pubN, vS1024 *privD);

#endif /* rsa_h */
