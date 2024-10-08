#include <linux/module.h>
#include <linux/init.h>
#include "pseudoCharDeviceDriver.h"

static int __init pcd_init(void)
{
    return 0;
}

static void __exit pcd_cleanup(void)
{}

module_init(pcd_init);
module_exit(pcd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan Wang，alan.wang@gmail.com");
MODULE_DESCRIPTION("A driver for a pseudo char device.");