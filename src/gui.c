#include "../headers/gui.h"

#ifdef __RAYGUI_INIT_
void
init_gui ()
{
  InitWindow (800, 400, "raygui - controls test suite");
  SetTargetFPS (60);

  bool showMessageBox = false;

  while (!WindowShouldClose ())
    {
      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing ();
      ClearBackground (GetColor (GuiGetStyle (DEFAULT, BACKGROUND_COLOR)));

      if (GuiButton ((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message"))
        showMessageBox = true;

      if (showMessageBox)
        {
          int result = GuiMessageBox ((Rectangle){ 85, 70, 250, 100 },
                                      "#191#Message Box",
                                      "Hi! This is a message!", "Nice;Cool");

          if (result >= 0)
            showMessageBox = false;
        }

      EndDrawing ();
    }

  CloseWindow ();
}
#endif
