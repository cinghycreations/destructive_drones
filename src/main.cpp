#include <memory.h>
#include <raylib.h>
#include <utility>
#include <array>
#include <filesystem>
#include <glm/glm.hpp>

#pragma optimize("",off)

struct Settings {
	float playerMovementSpeed = 0.1f;
	float itemSpawnDelay = 10.0f;
	int machinegunAmmo = 100;
	int laserAmmo = 100;
	int rocketlauncherAmmo = 10;
};

struct Content {
	Texture drone;
	Texture machinegun;
	Texture laser;
	Texture rocketlauncher;
	Texture crosshair;

	Texture percent100;
	Texture percent75;
	Texture percent50;
	Texture percent25;

	Content() {
		drone = LoadTexture("drone.png");
		machinegun = LoadTexture("machinegun.png");
		laser = LoadTexture("laser.png");
		rocketlauncher = LoadTexture("rocketlauncher.png");
		crosshair = LoadTexture("crosshair.png");

		percent100 = LoadTexture("100pc.png");
		percent75 = LoadTexture("75pc.png");
		percent50 = LoadTexture("50pc.png");
		percent25 = LoadTexture("25pc.png");
	}

	~Content() {
		UnloadTexture(drone);
		UnloadTexture(machinegun);
		UnloadTexture(laser);
		UnloadTexture(rocketlauncher);
		UnloadTexture(crosshair);

		UnloadTexture(percent100);
		UnloadTexture(percent75);
		UnloadTexture(percent50);
		UnloadTexture(percent25);
	}
};

enum class ItemType {
	None,
	MachineGun,
	Laser,
	RocketLauncher,
};

struct Bounds {
	glm::ivec2 position;
	glm::ivec2 size;
};

class Actor {
public:
	Bounds bounds;
};

class Item : public Actor {
public:
	ItemType type;
};

class Player : public Actor {
public:
	int playerIndex;
	int score = 0;
	float health = 1;
	float maxHealth = 1;
	glm::ivec2 crosshairPosition;
	std::chrono::steady_clock::time_point lastMovement;
	std::chrono::steady_clock::time_point lastShot;
	ItemType weapon = ItemType::None;
	int ammo = 0;
	int maxAmmo = 0;
};

class Projectile : public Actor {
public:
	glm::vec3 velocity;
};

class Level {
public:
	static constexpr int width = 64;
	static constexpr int height = 56;

	struct Tile
	{
		bool bedrock;
		float solidity;
	};

	struct ItemSpawn
	{
		glm::ivec2 position;
		ItemType type;
	};

	std::array< std::array< Tile, width >, height> tiles;
	std::vector<glm::ivec2> playerSpawns;
	std::vector<ItemSpawn> itemSpawns;

	Texture texture;

	Level() {
		testLevel();

		Image dummy_image = GenImageColor(width, height, BLACK);
		texture = LoadTextureFromImage(dummy_image);
		UnloadImage(dummy_image);

		refreshTexture();
	}

	~Level() {
		UnloadTexture(texture);
	}

	void refreshTexture() {
		std::array< std::array< glm::u8vec4, width >, height> pixels;

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				const Tile& tile = tiles.at(i).at(j);
				if (tile.solidity > 0) {
					pixels.at(i).at(j) = glm::u8vec4(0, 0, 0, 255);
				}
				else {
					pixels.at(i).at(j) = glm::u8vec4(255, 255, 255, 255);
				}
			}
		}

		UpdateTexture(texture, &pixels);
	}

private:
	void testLevel() {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				Tile tile;
				tile.bedrock = (i == 0 || i == height - 1 || j == 0 || j == width - 1);
				tile.solidity = tile.bedrock ? 1 : 0;
				tiles.at(i).at(j) = tile;
			}
		}

		for (int i = 20; i < 40; ++i) {
			for (int j = 20; j < 40; ++j) {
				Tile tile;
				tile.bedrock = false;
				tile.solidity = 1;
				tiles.at(i).at(j) = tile;
			}
		}

		playerSpawns.clear();
		playerSpawns.push_back(glm::ivec2(2, 2));
		playerSpawns.push_back(glm::ivec2(58, 2));
		playerSpawns.push_back(glm::ivec2(2, 50));
		playerSpawns.push_back(glm::ivec2(58, 50));

		itemSpawns.clear();
		itemSpawns.push_back(ItemSpawn{ glm::ivec2(31, 50), ItemType::MachineGun });
		itemSpawns.push_back(ItemSpawn{ glm::ivec2(9, 19), ItemType::Laser });
		itemSpawns.push_back(ItemSpawn{ glm::ivec2(49, 21), ItemType::RocketLauncher });
	}
};

class Session {
public:
	const Settings& settings;
	const Content& content;
	Level level;
	std::vector<Player> players;
	std::vector<Item> items;
	std::vector<Projectile> projectiles;

	Session(const Settings& _settings, const Content& _content, Level& _level) : settings(_settings), content(_content), level(_level) {
		for (const Level::ItemSpawn& spawn : level.itemSpawns) {
			Item item;
			item.bounds.position = spawn.position;
			item.bounds.size = glm::ivec2(4, 4);
			item.type = spawn.type;
			items.emplace_back(std::move(item));
		}
	}

	void addPlayer(const int index) {
		glm::ivec2 position = level.playerSpawns.front();

		Player player;
		player.bounds.position = position;
		player.bounds.size = glm::ivec2(4, 4);
		player.playerIndex = index;
		player.crosshairPosition = player.bounds.position;
		player.lastMovement = player.lastShot = std::chrono::steady_clock::now();
		players.emplace_back(std::move(player));
	}

	static bool collide(const Bounds& bounds0, const Bounds& bounds1) {
		const glm::ivec2 intersect_min = glm::max(bounds0.position, bounds1.position);
		const glm::ivec2 intersect_max = glm::min(bounds0.position + bounds0.size, bounds1.position + bounds1.size);
		const glm::ivec2 intersect_size = intersect_max - intersect_min;
		return intersect_size.x > 0 && intersect_size.y > 0;
	}

	static bool collide(const Bounds& bounds, const Level& level) {
		for (int i = bounds.position.y; i < bounds.position.y + bounds.size.y; ++i) {
			for (int j = bounds.position.x; j < bounds.position.x + bounds.size.x; ++j) {
				if (level.tiles.at(i).at(j).solidity > 0) {
					return true;
				}
			}
		}

		return false;
	}

	void update() {
		for (Player& player : players) {
			if (player.playerIndex == 0) {
				bool move_right = false;
				bool move_left = false;
				bool move_down = false;
				bool move_up = false;

				const auto delay = std::chrono::steady_clock::now() - player.lastMovement;
				if (delay >= std::chrono::milliseconds(long long(settings.playerMovementSpeed * 1000))) {
					move_right = IsKeyDown(KEY_D);
					move_left = IsKeyDown(KEY_A);
					move_down = IsKeyDown(KEY_S);
					move_up = IsKeyDown(KEY_W);
					player.lastMovement = std::chrono::steady_clock::now();
				}

				Bounds new_bounds = player.bounds;
				if (move_right) {
					new_bounds.position.x += 1;
				}
				else if (move_left) {
					new_bounds.position.x -= 1;
				}
				else if (move_down) {
					new_bounds.position.y += 1;
				}
				else if (move_up) {
					new_bounds.position.y -= 1;
				}

				if (!collide(new_bounds, level)) {
					player.bounds = new_bounds;
				}
			}

			for (auto iter = items.begin(); iter != items.end(); ++iter) {
				if (collide(player.bounds, iter->bounds)) {
					if (iter->type == ItemType::MachineGun) {
						player.weapon = ItemType::MachineGun;
						player.ammo = player.maxAmmo = settings.machinegunAmmo;
					}
					else if (iter->type == ItemType::Laser) {
						player.weapon = ItemType::Laser;
						player.ammo = player.maxAmmo = settings.laserAmmo;
					}
					else if (iter->type == ItemType::RocketLauncher) {
						player.weapon = ItemType::RocketLauncher;
						player.ammo = player.maxAmmo = settings.rocketlauncherAmmo;
					}

					items.erase(iter);
					break;
				}
			}
		}
	}

	void renderScene() {
		DrawTexture(level.texture, 0, 0, WHITE);

		for (const Player& player : players) {
			static const std::array<Color, 4> player_tints{ RED, YELLOW, GREEN, BLUE };
			DrawTexture(content.drone, player.bounds.position.x, player.bounds.position.y, player_tints.at(player.playerIndex));
			DrawTexture(content.crosshair, player.crosshairPosition.x, player.crosshairPosition.y, player_tints.at(player.playerIndex));
		}

		for (const Item& item : items) {
			if (item.type == ItemType::MachineGun) {
				DrawTexture(content.machinegun, item.bounds.position.x, item.bounds.position.y, WHITE);
			}
			else if (item.type == ItemType::Laser) {
				DrawTexture(content.laser, item.bounds.position.x, item.bounds.position.y, WHITE);
			}
			else if (item.type == ItemType::RocketLauncher) {
				DrawTexture(content.rocketlauncher, item.bounds.position.x, item.bounds.position.y, WHITE);
			}
		}
	}

	void renderUi() {
		for (const Player& player : players) {
			static const std::array<int, 4> offsets{1, 19, 36, 54};
			static const std::array<Color, 4> player_tints{ RED, YELLOW, GREEN, BLUE };

			const int offset = offsets.at(player.playerIndex);
			const Color tint = player_tints.at(player.playerIndex);

			{
				DrawTexture(content.drone, offset, 57, tint);

				const float healthpercent = player.health / player.maxHealth * 100.0f;

				if (healthpercent >= 99) {
					DrawTexture(content.percent100, offset, 62, tint);
				}
				else if (healthpercent >= 75) {
					DrawTexture(content.percent75, offset, 62, tint);
				}
				else if (healthpercent >= 50) {
					DrawTexture(content.percent50, offset, 62, tint);
				}
				else {
					DrawTexture(content.percent25, offset, 62, tint);
				}
			}

			if (player.weapon != ItemType::None) {
				if (player.weapon == ItemType::MachineGun) {
					DrawTexture(content.machinegun, offset + 5, 57, WHITE);
				}
				else if (player.weapon == ItemType::Laser) {
					DrawTexture(content.laser, offset + 5, 57, WHITE);
				}
				else if (player.weapon == ItemType::RocketLauncher) {
					DrawTexture(content.rocketlauncher, offset + 5, 57, WHITE);
				}

				const float ammopercent = float(player.ammo) / (player.maxAmmo) * 100.0f;

				if (ammopercent >= 99) {
					DrawTexture(content.percent100, offset + 5, 62, tint);
				}
				else if (ammopercent >= 75) {
					DrawTexture(content.percent75, offset + 5, 62, tint);
				}
				else if (ammopercent >= 50) {
					DrawTexture(content.percent50, offset + 5, 62, tint);
				}
				else {
					DrawTexture(content.percent25, offset + 5, 62, tint);
				}
			}
		}
	}
};

int main() {
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Destructive Drones");
	SetTargetFPS(60);

	Image drone = LoadImage("drone.png");
	Texture texture = LoadTextureFromImage(drone);

	Settings settings;
	Content content;
	Level level;
	Session session(settings, content, level);
	session.addPlayer(0);

	while (!WindowShouldClose()) {

		Camera2D camera;
		memset(&camera, 0, sizeof(Camera2D));
		camera.zoom = float(std::min(GetScreenWidth(), GetScreenHeight())) / 64.0f;

		if (IsKeyPressed(KEY_F6)) {
			session.addPlayer(1);
		}
		if (IsKeyPressed(KEY_F7)) {
			session.addPlayer(2);
		}
		if (IsKeyPressed(KEY_F8)) {
			session.addPlayer(3);
		}

		session.update();

		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(RAYWHITE);
		session.renderScene();
		session.renderUi();
		EndMode2D();
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
