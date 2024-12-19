# Copyright (c) 2024 Samy Francelet
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_BOARD_VLAD_PMIC_STARTUP pmic_startup.c)