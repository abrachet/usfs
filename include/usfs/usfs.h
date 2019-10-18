
#ifndef USFS_USFS_H
#define USFS_USFS_H

#define __CTOR __attribute__((constructor))

#define __CTOR_(prior) __attribute((constructor(prior)))

#define __DTOR __attribute__((destructor))

#endif  // USFS_USFS_H
