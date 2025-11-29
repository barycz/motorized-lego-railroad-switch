
#pragma once

namespace Ui {

void BeginWidget();
void EndWidget();

void Text(const char* format, ...);
void Separator();

void Init();
void Deinit();

}