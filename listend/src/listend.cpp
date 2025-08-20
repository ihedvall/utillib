/*
 * Copyright 2025 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "listenapp.h"
int main(int nof_arg, char* arg_list[]) {
  util::ListenApp app;
  return app.MainFunc(nof_arg, arg_list);
}
