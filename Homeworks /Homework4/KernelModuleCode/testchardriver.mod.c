#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x7ba18236, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x63fd0972, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xb9f9315, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xb8eec98b, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xc1166519, __VMLINUX_SYMBOL_STR(gpiod_unexport) },
	{ 0x76d7f249, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x22bfbf62, __VMLINUX_SYMBOL_STR(gpiod_export) },
	{ 0x1a224499, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x6ce2be1d, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x8fdf772a, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xfe3ca55b, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x2e703b21, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xd7b22e84, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xb16b8dfc, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x85825898, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xfa5bcf35, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x7f02188f, __VMLINUX_SYMBOL_STR(__msecs_to_jiffies) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "681BAE1F8142923F7999049");
