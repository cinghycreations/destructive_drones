#include <memory.h>
#include <raylib.h>
#include <utility>
#include <array>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <list>
#include <optional>

#pragma optimize("",off)

enum WeaponType {
	MachineGun,
	Laser,
	RocketLauncher,
};

struct WeaponSettings {
	int ammo;
	int maxAmmo;
	float shootDelay;
	float projectileSpeed;
	float projectileDamage;
	int blastRadius;
};

struct Settings {
	float playerMaxHealth;
	float playerSpeed;
	float playerCrosshairSpeed;
	float itemSpawnDelay;
	float tileHealth;
	std::array<WeaponSettings, 3> weapons;

	Settings() {
		playerMaxHealth = 100.0f;
		playerSpeed = 20.0f;
		playerCrosshairSpeed = 20.0f;
		itemSpawnDelay = 10.0f;
		tileHealth = 10.0f;
		weapons.at(WeaponType::MachineGun).ammo = 100;
		weapons.at(WeaponType::MachineGun).maxAmmo = 100;
		weapons.at(WeaponType::MachineGun).shootDelay = 0.2f;
		weapons.at(WeaponType::MachineGun).projectileSpeed = 50.0f;
		weapons.at(WeaponType::MachineGun).projectileDamage = 5.0f;
		weapons.at(WeaponType::MachineGun).blastRadius = 0;
		weapons.at(WeaponType::Laser).ammo = 20;
		weapons.at(WeaponType::Laser).maxAmmo = 20;
		weapons.at(WeaponType::Laser).shootDelay = 1.0f;
		weapons.at(WeaponType::Laser).projectileSpeed = 100.0f;
		weapons.at(WeaponType::Laser).projectileDamage = 25.0f;
		weapons.at(WeaponType::Laser).blastRadius = 0;
		weapons.at(WeaponType::RocketLauncher).ammo = 5;
		weapons.at(WeaponType::RocketLauncher).maxAmmo = 5;
		weapons.at(WeaponType::RocketLauncher).shootDelay = 2.0f;
		weapons.at(WeaponType::RocketLauncher).projectileSpeed = 50.0f;
		weapons.at(WeaponType::RocketLauncher).projectileDamage = 50.0f;
		weapons.at(WeaponType::RocketLauncher).blastRadius = 4;
	}
};

struct Content {
	Texture pixel;

	Texture drone;
	Texture machinegun;
	Texture laser;
	Texture rocketlauncher;
	Texture crosshair;

	Content() {
		pixel = LoadTexture("pixel.png");

		drone = LoadTexture("drone.png");
		machinegun = LoadTexture("machinegun.png");
		laser = LoadTexture("laser.png");
		rocketlauncher = LoadTexture("rocketlauncher.png");
		crosshair = LoadTexture("crosshair.png");
	}

	~Content() {
		UnloadTexture(pixel);

		UnloadTexture(drone);
		UnloadTexture(machinegun);
		UnloadTexture(laser);
		UnloadTexture(rocketlauncher);
		UnloadTexture(crosshair);
	}
};

enum ItemType {
	Weapon0,
	Weapon1,
	Weapon2,
	Weapon3,
	Weapon4,
	Weapon5,
	Weapon6,
	Weapon7,
};

struct Bounds {
	glm::ivec2 position;
	glm::ivec2 size;
};

class Actor {
public:
	Bounds bounds;

	Actor(const Bounds& _bounds) : bounds(_bounds) {}
};

class Item : public Actor {
public:
	ItemType type;

	Item(const Bounds& _bounds, const ItemType _type) : Actor(_bounds), type(_type) {}
};

class Player : public Actor {
public:
	int playerIndex;
	float health;
	int score = 0;
	glm::vec2 subpixelCrosshairPosition;
	glm::ivec2 crosshairPosition;
	double lastShot = 0;
	std::optional<WeaponType> weapon;
	int ammo = 0;
	glm::vec2 subpixelPosition;

	Player(const Bounds& _bounds, const int player_index, const float _health) : Actor(_bounds), playerIndex(player_index),
		subpixelCrosshairPosition(bounds.position), crosshairPosition(bounds.position), health(_health), subpixelPosition(bounds.position) { }
};

class Projectile : public Actor {
public:
	Projectile(const Bounds& _bounds, const int owner_player_index, const int from_weapon, const glm::vec2& subpixel_velocity) :
		Actor(_bounds), ownerPlayerIndex(owner_player_index), fromWeapon(from_weapon), subpixelVelocity(subpixel_velocity) {
		subpixelPosition = glm::vec2(bounds.position) + glm::vec2(bounds.size) * 0.5f;
	}

	int ownerPlayerIndex;
	int fromWeapon;
	glm::vec2 subpixelPosition;
	glm::vec2 subpixelVelocity;
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

	const Settings& settings;
	std::array< std::array< Tile, width >, height> tiles;
	std::vector<glm::ivec2> playerSpawns;
	std::vector<ItemSpawn> itemSpawns;

	Texture texture;
	bool textureDirty = false;

	Level(const Settings& _settings) : settings(_settings) {
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
				tile.solidity = tile.bedrock ? settings.tileHealth : 0;
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
		itemSpawns.push_back(ItemSpawn{ glm::ivec2(10, 2), ItemType::Weapon0 });
		itemSpawns.push_back(ItemSpawn{ glm::ivec2(9, 19), ItemType::Weapon1 });
		itemSpawns.push_back(ItemSpawn{ glm::ivec2(49, 21), ItemType::Weapon2 });
	}
};

class Session {
public:
	const Settings& settings;
	const Content& content;
	Level level;
	std::vector<Player> players;
	std::list<Item> items;
	std::list<Projectile> projectiles;

	Session(const Settings& _settings, const Content& _content, Level& _level) : settings(_settings), content(_content), level(_level) {
		for (const Level::ItemSpawn& spawn : level.itemSpawns) {
			Bounds bounds{ spawn.position, glm::ivec2(4,4) };
			Item item(bounds, spawn.type);
			items.emplace_back(std::move(item));
		}
	}

	void addPlayer(const int index) {
		glm::ivec2 position = level.playerSpawns.front();
		Bounds bounds{ position, glm::ivec2(4,4) };
		Player player(bounds, index, settings.playerMaxHealth);
		players.emplace_back(std::move(player));
	}

	static bool inLevel(const glm::ivec2& point, const Level& level) {
		return point.x >= 0 && point.x < level.width && point.y >= 0 && point.y < level.height;
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
				if (inLevel(glm::ivec2(i, j), level) && level.tiles.at(i).at(j).solidity > 0) {
					return true;
				}
			}
		}

		return false;
	}

	static bool collide(const glm::ivec2& point, const Bounds& bounds) {
		return point.x >= bounds.position.x && point.x < bounds.position.x + bounds.size.x &&
			point.y >= bounds.position.y && point.y < bounds.position.y + bounds.size.y;
	}

	static bool collide(const glm::ivec2& point, const Level& level) {
		return inLevel(point, level) && level.tiles.at(point.y).at(point.x).solidity > 0;
	}

	static std::vector<glm::ivec2> rasterizeLine(const glm::vec2& segment_start, const glm::vec2& segment_end) {
		// Temporary implementation
		std::vector<glm::ivec2> result;
		const int steps = std::max(int(glm::distance(segment_start, segment_end) * 2), 1);

		for (int step = 0; step <= steps; step++) {
			const glm::ivec2 position = glm::ivec2(glm::lerp(segment_start, segment_end, float(step) / float(steps)));
			result.push_back(position);
		}

		return result;
	}

	void update() {
		for (Player& player : players) {
			glm::vec2 move_direction(0, 0);
			glm::vec2 move_crosshair_direction(0, 0);
			bool fire = false;

			if (player.playerIndex == 0) {
				if (IsKeyDown(KEY_D)) {
					move_direction = glm::vec2(1, 0);
				}
				else if (IsKeyDown(KEY_A)) {
					move_direction = glm::vec2(-1, 0);
				}
				else if (IsKeyDown(KEY_S)) {
					move_direction = glm::vec2(0, 1);
				}
				else if (IsKeyDown(KEY_W)) {
					move_direction = glm::vec2(0, -1);
				}

				if (IsKeyDown(KEY_RIGHT)) {
					move_crosshair_direction = glm::vec2(1, 0);
				}
				else if (IsKeyDown(KEY_LEFT)) {
					move_crosshair_direction = glm::vec2(-1, 0);
				}
				else if (IsKeyDown(KEY_DOWN)) {
					move_crosshair_direction = glm::vec2(0, 1);
				}
				else if (IsKeyDown(KEY_UP)) {
					move_crosshair_direction = glm::vec2(0, -1);
				}

				fire = IsKeyDown(KEY_SPACE);
			}

			{
				glm::vec2 new_subpixel_position = player.subpixelPosition + move_direction * settings.playerSpeed * GetFrameTime();
				Bounds new_bounds{ glm::ivec2(new_subpixel_position), player.bounds.size };

				if (!collide(new_bounds, level)) {
					player.subpixelPosition = new_subpixel_position;
					player.bounds = new_bounds;
				}
			}

			{
				player.subpixelCrosshairPosition = player.subpixelCrosshairPosition + move_crosshair_direction * settings.playerCrosshairSpeed * GetFrameTime();
				player.crosshairPosition = glm::ivec2(player.subpixelCrosshairPosition);
			}

			if (player.weapon.has_value()) {
				const WeaponSettings& weapon_settings = settings.weapons.at(*player.weapon);
				if (fire && player.ammo > 0 && GetTime() - player.lastShot >= weapon_settings.shootDelay) {
					const glm::vec2 velocity = glm::normalize(glm::vec2(player.crosshairPosition) - glm::vec2(player.bounds.position)) * weapon_settings.projectileSpeed;
					const glm::ivec2 projectile_position = player.bounds.position + player.bounds.size / 2;
					Projectile projectile(Bounds{ projectile_position, glm::ivec2(1,1) }, player.playerIndex, *player.weapon, velocity);
					projectiles.emplace_back(std::move(projectile));

					player.ammo -= 1;
					if (player.ammo == 0) {
						player.weapon.reset();
					}

					player.lastShot = GetTime();
				}
			}

			for (auto item_iter = items.begin(); item_iter != items.end(); ++item_iter) {
				if (collide(player.bounds, item_iter->bounds)) {
					if (item_iter->type >= ItemType::Weapon0 && item_iter->type <= ItemType::Weapon7) {
						player.weapon = WeaponType(item_iter->type - ItemType::Weapon0);
						player.ammo = std::max<int>(player.ammo + settings.weapons.at(*player.weapon).ammo, settings.weapons.at(*player.weapon).maxAmmo);
					}

					item_iter = items.erase(item_iter);
					break;
				}
			}
		}

		for (auto projectile_iter = projectiles.begin(); projectile_iter != projectiles.end();) {
			Projectile& projectile = *projectile_iter;
			const glm::vec2 start_position = projectile.subpixelPosition;

			if (!inLevel(start_position, level)) {
				projectile_iter = projectiles.erase(projectile_iter);
				continue;
			}

			const glm::vec2 end_position = projectile.subpixelPosition + projectile.subpixelVelocity * GetFrameTime();
			const std::vector<glm::ivec2> rasterized_line = rasterizeLine(start_position, end_position);
			std::optional<glm::ivec2> hit = std::nullopt;

			for (const glm::ivec2& pixel : rasterized_line) {
				if (collide(pixel, level)) {
					hit = pixel;
					break;
				}

				for (Player& player : players) {
					if (projectile.ownerPlayerIndex == player.playerIndex) {
						continue;
					}

					if (collide(pixel, player.bounds)) {
						hit = pixel;
						break;
					}
				}
			}

			if (hit.has_value()) {
				const WeaponSettings& weapon_settings = settings.weapons.at(projectile.fromWeapon);
				const glm::ivec2 blast_min = *hit - glm::ivec2(weapon_settings.blastRadius, weapon_settings.blastRadius);
				const glm::ivec2 blast_max = *hit + glm::ivec2(weapon_settings.blastRadius, weapon_settings.blastRadius);

				std::array<bool, 8> player_affected;
				player_affected.fill(false);

				for (int hit_x = blast_min.x; hit_x <= blast_max.x; ++hit_x) {
					for (int hit_y = blast_min.y; hit_y <= blast_max.y; ++hit_y) {
						const glm::ivec2 blast_hit(hit_x, hit_y);
						if (!inLevel(blast_hit, level) || glm::distance(glm::vec2(*hit), glm::vec2(blast_hit)) > float(weapon_settings.blastRadius)) {
							continue;
						}

						Level::Tile& tile = level.tiles.at(blast_hit.y).at(blast_hit.x);
						if (!tile.bedrock) {
							tile.solidity -= weapon_settings.projectileDamage;
							if (tile.solidity <= 0) {
								level.textureDirty = true;
							}
						}

						for (Player& player : players) {
							if (player_affected.at(player.playerIndex)) {
								continue;
							}

							if (collide(blast_hit, player.bounds)) {
								player_affected.at(player.playerIndex) = true;
								player.health -= weapon_settings.projectileDamage;
							}
						}
					}
				}

				projectile_iter = projectiles.erase(projectile_iter);
			}
			else {
				projectile.bounds.position = end_position;
				projectile.subpixelPosition = end_position;
				++projectile_iter;
			}
		}
	}

	void renderScene() {
		if (level.textureDirty) {
			level.refreshTexture();
			level.textureDirty = false;
		}

		DrawTexture(level.texture, 0, 0, WHITE);

		for (const Player& player : players) {
			static const std::array<Color, 4> player_tints{ RED, YELLOW, GREEN, BLUE };
			DrawTexture(content.drone, player.bounds.position.x, player.bounds.position.y, player_tints.at(player.playerIndex));
			DrawTexture(content.crosshair, player.crosshairPosition.x, player.crosshairPosition.y, player_tints.at(player.playerIndex));
		}

		for (const Item& item : items) {
			if (item.type == ItemType::Weapon0) {
				DrawTexture(content.machinegun, item.bounds.position.x, item.bounds.position.y, WHITE);
			}
			else if (item.type == ItemType::Weapon1) {
				DrawTexture(content.laser, item.bounds.position.x, item.bounds.position.y, WHITE);
			}
			else if (item.type == ItemType::Weapon2) {
				DrawTexture(content.rocketlauncher, item.bounds.position.x, item.bounds.position.y, WHITE);
			}
		}

		for (const Projectile& projectile : projectiles) {
			DrawTexture(content.pixel, projectile.bounds.position.x, projectile.bounds.position.y, GRAY);
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

				const float healthpercent = player.health / settings.playerMaxHealth * 100.0f;

				if (healthpercent > 0) {
					DrawTexture(content.pixel, offset, 62, tint);
				}
				if (healthpercent > 25) {
					DrawTexture(content.pixel, offset + 1, 62, tint);
				}
				if (healthpercent > 50) {
					DrawTexture(content.pixel, offset + 2, 62, tint);
				}
				if (healthpercent > 75) {
					DrawTexture(content.pixel, offset + 3, 62, tint);
				}
			}

			if (player.weapon.has_value()) {
				if (player.weapon == WeaponType::MachineGun) {
					DrawTexture(content.machinegun, offset + 5, 57, WHITE);
				}
				else if (player.weapon == WeaponType::Laser) {
					DrawTexture(content.laser, offset + 5, 57, WHITE);
				}
				else if (player.weapon == WeaponType::RocketLauncher) {
					DrawTexture(content.rocketlauncher, offset + 5, 57, WHITE);
				}

				const float ammopercent = float(player.ammo) / (settings.weapons.at(*player.weapon).maxAmmo) * 100.0f;

				if (ammopercent > 0) {
					DrawTexture(content.pixel, offset + 5, 62, tint);
				}
				if (ammopercent > 25) {
					DrawTexture(content.pixel, offset + 6, 62, tint);
				}
				if (ammopercent > 50) {
					DrawTexture(content.pixel, offset + 7, 62, tint);
				}
				if (ammopercent > 75) {
					DrawTexture(content.pixel, offset + 8, 62, tint);
				}
			}
		}
	}
};

int main() {
	InitWindow(1280, 720, "Destructive Drones");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);

	Image drone = LoadImage("drone.png");
	Texture texture = LoadTextureFromImage(drone);

	Settings settings;
	Content content;
	Level level(settings);
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
