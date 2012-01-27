How to build


1. Get android open source.
    : version info - Android froyo 2.2 (android-cts-2.2_r2)
    ( Download site : http://source.android.com )

2. Overwrite modules that you want to build.

3. Add the following lines at the end of build/target/board/generic/BoardConfig.mk

BOARD_HAVE_BLUETOOTH := true
BT_ALT_STACK := true
BRCM_BT_USE_BTL_IF := true
BRCM_BTL_INCLUDE_A2DP := true
BRCM_BTL_INCLUDE_OBEX := true
BRCM_BTL_OBEX_USE_DBUS := true

4. make