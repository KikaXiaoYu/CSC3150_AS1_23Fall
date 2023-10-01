#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x182197f3, "module_layout" },
	{ 0xd9f448da, "wake_up_process" },
	{ 0x9fc1fb2a, "kthread_create_on_node" },
	{ 0xc5850110, "printk" },
	{ 0x8b20bbb5, "kernel_clone" },
	{ 0x38c58832, "current_task" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x2d15d1b6, "do_execve" },
	{ 0x85416d23, "getname_kernel" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "231E813952548C2834167C4");
