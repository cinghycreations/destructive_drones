#include <raylib.h>

int main() {

	InitWindow(64, 64, "Destructive Drones");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawCircle(32, 32, 10, RED);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
