#include <memory.h>
#include <raylib.h>
#include <utility>
#include <array>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <list>
#include <optional>
#include <random>
#include <queue>
#include <fstream>
#include <string>

enum WeaponType {
	MachineGun,
	Shotgun,
	RocketLauncher,
};

struct WeaponSettings {
	int maxAmmo;
	float shootDelay;
	float projectileSpeed;
	float projectileDamage;
	int projectileCount;
	float projectileSpread;
	float projectileLife;
	int blastRadius;
	bool shakeOnHit;
	int soundIndex;
};

struct Settings {
	float playerMaxHealth;
	float playerSpeed;
	float playerCrosshairSpeed;
	float itemSpawnDelay;
	float tileHealth;
	int scoreForWin;
	float respawnTime;
	float cameraShakeStrength;
	float cameraShakeTime;
	std::array<Color, 4> playerTints;
	std::array<WeaponSettings, 3> weapons;

	Settings() {
		playerMaxHealth = 100.0f;
		playerSpeed = 20.0f;
		playerCrosshairSpeed = 20.0f;
		itemSpawnDelay = 5.0f;
		tileHealth = 10.0f;
		scoreForWin = 20;
		respawnTime = 3;
		cameraShakeStrength = 1.0f;
		cameraShakeTime = 0.5f;
		playerTints.at(0) = RED;
		playerTints.at(1) = YELLOW;
		playerTints.at(2) = GREEN;
		playerTints.at(3) = BLUE;
		weapons.at(WeaponType::MachineGun).maxAmmo = 40;
		weapons.at(WeaponType::MachineGun).shootDelay = 0.2f;
		weapons.at(WeaponType::MachineGun).projectileSpeed = 50.0f;
		weapons.at(WeaponType::MachineGun).projectileDamage = 5.0f;
		weapons.at(WeaponType::MachineGun).projectileCount = 1;
		weapons.at(WeaponType::MachineGun).projectileSpread = 0;
		weapons.at(WeaponType::MachineGun).projectileLife = FLT_MAX;
		weapons.at(WeaponType::MachineGun).blastRadius = 0;
		weapons.at(WeaponType::MachineGun).shakeOnHit = false;
		weapons.at(WeaponType::MachineGun).soundIndex = 0;
		weapons.at(WeaponType::Shotgun).maxAmmo = 20;
		weapons.at(WeaponType::Shotgun).shootDelay = 1.0f;
		weapons.at(WeaponType::Shotgun).projectileSpeed = 50.0f;
		weapons.at(WeaponType::Shotgun).projectileDamage = 10.0f;
		weapons.at(WeaponType::Shotgun).projectileCount = 7;
		weapons.at(WeaponType::Shotgun).projectileSpread = 60;
		weapons.at(WeaponType::Shotgun).projectileLife = 1.0f;
		weapons.at(WeaponType::Shotgun).blastRadius = 0;
		weapons.at(WeaponType::Shotgun).shakeOnHit = false;
		weapons.at(WeaponType::Shotgun).soundIndex = 0;
		weapons.at(WeaponType::RocketLauncher).maxAmmo = 5;
		weapons.at(WeaponType::RocketLauncher).shootDelay = 1.0f;
		weapons.at(WeaponType::RocketLauncher).projectileSpeed = 50.0f;
		weapons.at(WeaponType::RocketLauncher).projectileDamage = 50.0f;
		weapons.at(WeaponType::RocketLauncher).projectileCount = 1;
		weapons.at(WeaponType::RocketLauncher).projectileSpread = 0;
		weapons.at(WeaponType::RocketLauncher).projectileLife = FLT_MAX;
		weapons.at(WeaponType::RocketLauncher).blastRadius = 4;
		weapons.at(WeaponType::RocketLauncher).shakeOnHit = true;
		weapons.at(WeaponType::RocketLauncher).soundIndex = 1;
	}
};

struct Content {
	Texture pixel;

	Texture drone;
	Texture machinegun;
	Texture laser;
	Texture rocketlauncher;

	Texture button_back;
	Texture button_credits;
	Texture button_four;
	Texture button_help;
	Texture button_one;
	Texture button_play;
	Texture button_three;
	Texture button_two;
	Texture button_zero;
	Texture credits;
	Texture help1;
	Texture help2;
	Texture select_players;
	Texture splash;
	Texture rankings;

	Sound menuSound;
	Sound reloadSound;
	std::array<Sound, 2> weaponSounds;

	std::vector<Texture> menuVideo;

	Content() {
		pixel = LoadTexture("pixel.png");

		drone = LoadTexture("drone.png");
		machinegun = LoadTexture("machinegun.png");
		laser = LoadTexture("laser.png");
		rocketlauncher = LoadTexture("rocketlauncher.png");

		button_back = LoadTexture("ui/button_back.png");
		button_credits = LoadTexture("ui/button_credits.png");
		button_four = LoadTexture("ui/button_four.png");
		button_help = LoadTexture("ui/button_help.png");
		button_one = LoadTexture("ui/button_one.png");
		button_play = LoadTexture("ui/button_play.png");
		button_three = LoadTexture("ui/button_three.png");
		button_two = LoadTexture("ui/button_two.png");
		button_zero = LoadTexture("ui/button_zero.png");
		credits = LoadTexture("ui/credits.png");
		help1 = LoadTexture("ui/help1.png");
		help2 = LoadTexture("ui/help2.png");
		select_players = LoadTexture("ui/select_players.png");
		splash = LoadTexture("ui/splash.png");
		rankings = LoadTexture("ui/rankings.png");

		menuSound = LoadSound("menu.mp3");
		reloadSound = LoadSound("reload.mp3");
		weaponSounds.at(0) = LoadSound("shot.mp3");
		weaponSounds.at(1) = LoadSound("rocket.mp3");

		int i = 1;
		while (true) {
			std::array<char, 64> filename;
			sprintf_s(filename.data(), filename.size(), "menu%04d.png", i);
			const std::filesystem::path path = std::filesystem::path("video") / filename.data();

			if (std::filesystem::exists(path)) {
				Texture frame = LoadTexture(path.string().c_str());
				menuVideo.emplace_back(std::move(frame));
				++i;
			}
			else {
				break;
			}
		}
	}

	~Content() {
		UnloadTexture(pixel);

		UnloadTexture(drone);
		UnloadTexture(machinegun);
		UnloadTexture(laser);
		UnloadTexture(rocketlauncher);

		UnloadTexture(button_back);
		UnloadTexture(button_credits);
		UnloadTexture(button_four);
		UnloadTexture(button_help);
		UnloadTexture(button_one);
		UnloadTexture(button_play);
		UnloadTexture(button_three);
		UnloadTexture(button_two);
		UnloadTexture(button_zero);
		UnloadTexture(credits);
		UnloadTexture(help1);
		UnloadTexture(help2);
		UnloadTexture(select_players);
		UnloadTexture(splash);
		UnloadTexture(rankings);

		for (Texture& frame : menuVideo) {
			UnloadTexture(frame);
		}
		menuVideo.clear();

		UnloadSound(menuSound);
		UnloadSound(reloadSound);
		for (Sound& sound : weaponSounds) {
			UnloadSound(sound);
		}
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

struct Pathfinding {
	struct TilePath {
		int shortestPath;
		glm::ivec2 backDirection;
	};

	std::vector< std::vector<TilePath> > tilePaths;
};

class Player : public Actor {
public:
	int playerIndex;
	bool ai;
	float health;
	int score = 0;
	double lastShot = 0;
	std::optional<WeaponType> weapon;
	int ammo = 0;
	glm::vec2 subpixelPosition;
	Pathfinding pathfinding;

	Player(const Bounds& _bounds, const int player_index, const bool _ai, const float _health) : Actor(_bounds), playerIndex(player_index), ai(_ai), health(_health), subpixelPosition(bounds.position) { }
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

	Level(const Settings& _settings, const std::filesystem::path& path) : settings(_settings) {
		//testLevel();
		loadLevel(path);

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
					pixels.at(i).at(j) = glm::u8vec4(255, 255, 255, 255);
				}
				else {
					pixels.at(i).at(j) = glm::u8vec4(0, 0, 0, 255);
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

	void loadLevel(const std::filesystem::path& path) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				tiles.at(i).at(j).bedrock = false;
				tiles.at(i).at(j).solidity = 0;
			}
		}
		playerSpawns.clear();
		itemSpawns.clear();

		std::ifstream stream(path);

		std::string line;
		for (int i = 0; i < height; ++i) {
			std::getline(stream, line);
			int j = 0;

			auto token_start = line.begin();
			for (auto iter = std::next(line.begin()); ; ++iter) {
				if (iter == line.end() || *iter == ',') {
					const std::string token(token_start, iter);
					const int tile = std::stoi(token);

					if (tile == 0) {
						tiles.at(i).at(j).bedrock = false;
						tiles.at(i).at(j).solidity = settings.tileHealth;
					}
					else if (tile == 1) {
						tiles.at(i).at(j).bedrock = true;
						tiles.at(i).at(j).solidity = settings.tileHealth;
					}
					else if (tile == 2) {
						playerSpawns.push_back(glm::ivec2(j, i));
					}
					else if (tile == 4) {
						itemSpawns.push_back(ItemSpawn{ glm::ivec2(j, i), ItemType::Weapon0 });
					}
					else if (tile == 5) {
						itemSpawns.push_back(ItemSpawn{ glm::ivec2(j, i), ItemType::Weapon1 });
					}
					else if (tile == 6) {
						itemSpawns.push_back(ItemSpawn{ glm::ivec2(j, i), ItemType::Weapon2 });
					}

					++j;
					if (iter == line.end()) {
						break;
					}
					else {
						token_start = std::next(iter);
					}
				}
			}
		}
	}
};

class CameraShake
{
public:
	const Settings& settings;

	CameraShake(const Settings& _settings) : settings(_settings) {
	}

	void shake() {
		const double now = GetTime();
		if (now >= startTime && now < endTime) {
			return;
		}

		startTime = now;
		endTime = startTime + settings.cameraShakeTime;
	}

	void updateCamera(Camera2D& camera) {
		const double now = GetTime();
		const double progress = (now - startTime) / (endTime - startTime);
		if (progress >= 0 && progress < 1) {
			const float angle = glm::radians(float(progress) * 360);
			const float dx = std::floor(std::cos(angle) * settings.cameraShakeStrength);
			const float dy = std::floor(std::sin(angle) * settings.cameraShakeStrength);
			camera.target = Vector2{ dx, dy };
		}
		else {
			camera.target = Vector2{ 0, 0 };
		}
	}

private:

	double startTime = -1;
	double endTime = 0;
};

class Session {
public:
	struct Respawn
	{
		enum RespawnType {
			Player,
			Item,
		};

		RespawnType type;
		double respawnTime;

		int playerIndex;

		ItemType itemType;
		Bounds itemBounds;
	};

	const Settings& settings;
	const Content& content;
	Level level;
	std::list<Player> players;
	std::list<Item> items;
	std::list<Projectile> projectiles;
	std::list<Respawn> respawns;
	CameraShake cameraShake;

	Session(const Settings& _settings, const Content& _content, Level& _level) : settings(_settings), content(_content), level(_level), cameraShake(settings) {
		for (const Level::ItemSpawn& spawn : level.itemSpawns) {
			Bounds bounds{ spawn.position, glm::ivec2(4,4) };
			Item item(bounds, spawn.type);
			items.emplace_back(std::move(item));
		}
	}

	glm::ivec2 findRespawnPosition() {
		std::vector<int> spawn_indices;
		for (int i = 0; i < level.playerSpawns.size(); ++i) {
			spawn_indices.push_back(i);
		}

		std::random_device random_device;
		std::mt19937 random_generator(random_device());
		std::shuffle(spawn_indices.begin(), spawn_indices.end(), random_generator);

		for (int spawn_index : spawn_indices) {
			const glm::ivec2 position = level.playerSpawns.at(spawn_index);
			Bounds bounds{ position, glm::ivec2(4,4) };
			bool occupied = false;

			for (const Player& other_player : players) {
				if (collide(bounds, other_player.bounds)) {
					occupied = true;
					break;
				}
			}

			if (!occupied) {
				return position;
			}
		}

		TraceLog(LOG_ERROR, "Couldn't find spawn position!");
		return glm::ivec2(-1, -1);
	}

	void addPlayer(const int index, const bool ai) {
		const glm::ivec2 position = findRespawnPosition();
		Bounds bounds{ position, glm::ivec2(4,4) };
		Player player(bounds, index, ai, settings.playerMaxHealth);
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
				if (inLevel(glm::ivec2(j, i), level) && level.tiles.at(i).at(j).solidity > 0) {
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

	static void updatePathfinding(Player& player, const Level& level) {
		if (player.pathfinding.tilePaths.empty()) {
			player.pathfinding.tilePaths.resize(level.height);
			for (int i = 0; i < level.height; ++i) {
				player.pathfinding.tilePaths.at(i).resize(level.width, Pathfinding::TilePath{-1, glm::ivec2(0, 0)});
			}
		}

		for (int i = 0; i < level.height; ++i) {
			for (int j = 0; j < level.width; ++j) {
				player.pathfinding.tilePaths.at(i).at(j) = Pathfinding::TilePath{ -1, glm::ivec2(0, 0) };
			}
		}

		std::queue<glm::ivec2> bfsQueue;
		player.pathfinding.tilePaths.at(player.bounds.position.y).at(player.bounds.position.x) = Pathfinding::TilePath{ 0, glm::ivec2(0, -0) };
		bfsQueue.push(player.bounds.position);

		while (!bfsQueue.empty()) {
			glm::ivec2 position = bfsQueue.front();
			bfsQueue.pop();

			const Pathfinding::TilePath& current_tilepath = player.pathfinding.tilePaths.at(position.y).at(position.x);

			for (glm::ivec2 delta : { glm::ivec2(-1, 0), glm::ivec2(1, 0), glm::ivec2(0, -1), glm::ivec2(0, 1) }) {
				Bounds new_bounds = player.bounds;
				new_bounds.position = position + delta;

				if (!inLevel(new_bounds.position, level)) {
					continue;
				}

				Pathfinding::TilePath& next_tilepath = player.pathfinding.tilePaths.at(new_bounds.position.y).at(new_bounds.position.x);

				if ((next_tilepath.shortestPath == -1 || current_tilepath.shortestPath + 1 < next_tilepath.shortestPath) && !collide(new_bounds, level)) {
					next_tilepath.shortestPath = current_tilepath.shortestPath + 1;
					next_tilepath.backDirection = -delta;
					bfsQueue.push(new_bounds.position);
				}
			}
		}
	}

	void aiPlayer(Player& player, glm::vec2& move_direction, glm::vec2& shoot_direction, bool& fire) {
		move_direction = glm::vec2(0, 0);
		shoot_direction = glm::vec2(0, 0);
		fire = false;

		//const int turn = int(GetTime() / 0.5f);
		//if (player.pathfinding.tilePaths.empty() || turn == player.playerIndex) {
		updatePathfinding(player, level);
		//}

		if (!player.weapon.has_value()) {
			int nearest_weapon_distance = -1;
			glm::ivec2 nearest_weapon_position(-1, -1);

			for (const Item& item : items) {
				if (item.type >= ItemType::Weapon0 && item.type <= ItemType::Weapon7) {
					for (int i = 0; i < item.bounds.size.y; ++i) {
						for (int j = 0; j < item.bounds.size.x; ++j) {
							const auto& tilepath = player.pathfinding.tilePaths.at(item.bounds.position.y + i).at(item.bounds.position.x + j);
							if (tilepath.shortestPath != -1 && (nearest_weapon_distance == -1 || tilepath.shortestPath < nearest_weapon_distance)) {
								nearest_weapon_distance = tilepath.shortestPath;
								nearest_weapon_position = glm::ivec2(item.bounds.position.x + j, item.bounds.position.y + i);
							}
						}
					}

				}
			}

			if (nearest_weapon_distance != -1) {
				glm::ivec2 direction;

				glm::ivec2 current_position = nearest_weapon_position;
				while (true) {
					const auto& tile_path = player.pathfinding.tilePaths.at(current_position.y).at(current_position.x);
					if (current_position == player.bounds.position || tile_path.shortestPath == 0) {
						break;
					}

					direction = -tile_path.backDirection;
					current_position = current_position + tile_path.backDirection;
				}

				move_direction = glm::normalize(glm::vec2(direction));
			}
		}
		else {
			const glm::ivec2 player_center = player.bounds.position + player.bounds.size / 2;

			int nearest_player_distance = -1;
			const Player* nearest_player = nullptr;

			for (const Player& other_player : players) {
				if (player.playerIndex != other_player.playerIndex && other_player.health > 0) {
					const auto& tile_path = player.pathfinding.tilePaths.at(other_player.bounds.position.y).at(other_player.bounds.position.x);

					if (tile_path.shortestPath != -1 && (nearest_player_distance == -1 || tile_path.shortestPath < nearest_player_distance)) {
						nearest_player_distance = tile_path.shortestPath;
						nearest_player = &other_player;
					}
				}
			}

			if (nearest_player_distance != -1) {
				std::vector<glm::ivec2> sight = rasterizeLine(player_center, nearest_player->bounds.position + nearest_player->bounds.size / 2);

				bool visible = true;
				for (const glm::ivec2& sight_point : sight) {
					if (collide(sight_point, level)) {
						visible = false;
						break;
					}
				}

				if (visible) {
					shoot_direction = glm::normalize(glm::vec2(nearest_player->bounds.position + nearest_player->bounds.size / 2) - glm::vec2(player_center));
					fire = true;
				}
				else {
					glm::ivec2 direction;

					glm::ivec2 current_position = nearest_player->bounds.position;
					while (true) {
						const auto& tile_path = player.pathfinding.tilePaths.at(current_position.y).at(current_position.x);
						if (current_position == player.bounds.position || tile_path.shortestPath == 0) {
							break;
						}

						direction = -tile_path.backDirection;
						current_position = current_position + tile_path.backDirection;
					}

					move_direction = glm::normalize(glm::vec2(direction));
				}
			}
		}
	}

	void humanPlayer(Player& player, glm::vec2& move_direction, glm::vec2& shoot_direction, bool& fire) {
		move_direction = glm::vec2(0, 0);
		shoot_direction = glm::vec2(0, 0);
		fire = false;

		if (IsGamepadAvailable(player.playerIndex)) {
			const float deadzone = 0.2f;
			{

				const float axis = GetGamepadAxisMovement(player.playerIndex, GAMEPAD_AXIS_LEFT_X);
				if (std::abs(axis) >= deadzone) {
					move_direction.x = axis;
				}
			}

			{
				const float axis = GetGamepadAxisMovement(player.playerIndex, GAMEPAD_AXIS_LEFT_Y);
				if (std::abs(axis) >= deadzone) {
					move_direction.y = axis;
				}
			}

			{
				const float axis = GetGamepadAxisMovement(player.playerIndex, GAMEPAD_AXIS_RIGHT_X);
				if (std::abs(axis) >= deadzone) {
					shoot_direction.x = axis;
				}
			}

			{
				const float axis = GetGamepadAxisMovement(player.playerIndex, GAMEPAD_AXIS_RIGHT_Y);
				if (std::abs(axis) >= deadzone) {
					shoot_direction.y = axis;
				}
			}

			if (glm::length(move_direction) > 0) {
				move_direction = glm::normalize(move_direction);
			}

			if (glm::length(shoot_direction) > 0) {
				shoot_direction = glm::normalize(shoot_direction);
			}

			fire = IsGamepadButtonDown(player.playerIndex, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
		}
	}

	void update() {
		for (Player& player : players) {
			if (player.health <= 0) {
				continue;
			}

			glm::vec2 move_direction(0, 0);
			glm::vec2 shoot_direction(0, 0);
			bool fire = false;

			if (player.ai) {
				aiPlayer(player, move_direction, shoot_direction, fire);
			}
			else {
				humanPlayer(player, move_direction, shoot_direction, fire);
			}

			{
				glm::vec2 new_subpixel_position = player.subpixelPosition + move_direction * settings.playerSpeed * GetFrameTime();
				Bounds new_bounds{ glm::ivec2(new_subpixel_position), player.bounds.size };

				if (!collide(new_bounds, level)) {
					player.subpixelPosition = new_subpixel_position;
					player.bounds = new_bounds;
				}
			}

			if (player.weapon.has_value()) {
				const WeaponSettings& weapon_settings = settings.weapons.at(*player.weapon);
				if (fire && player.ammo > 0 && GetTime() - player.lastShot >= weapon_settings.shootDelay) {
					for (int i = 0; i < weapon_settings.projectileCount; ++i) {
						const float angle_random = (float(i) / weapon_settings.projectileCount) * 2 - 1;
						const float angle = glm::radians(angle_random * weapon_settings.projectileSpread);
						const glm::vec2 velocity = glm::rotate(shoot_direction, angle) * weapon_settings.projectileSpeed;

						const glm::ivec2 projectile_position = player.bounds.position + player.bounds.size / 2;
						Projectile projectile(Bounds{ projectile_position, glm::ivec2(1,1) }, player.playerIndex, *player.weapon, velocity);
						projectiles.emplace_back(std::move(projectile));
					}

					player.ammo -= 1;
					if (player.ammo == 0) {
						player.weapon.reset();
					}

					PlaySound(content.weaponSounds.at(weapon_settings.soundIndex));

					player.lastShot = GetTime();
				}
			}

			for (auto item_iter = items.begin(); item_iter != items.end(); ++item_iter) {
				if (collide(player.bounds, item_iter->bounds)) {
					if (item_iter->type >= ItemType::Weapon0 && item_iter->type <= ItemType::Weapon7) {
						const WeaponType weapon_type = WeaponType(item_iter->type - ItemType::Weapon0);
						player.weapon = weapon_type;
						player.ammo = settings.weapons.at(*player.weapon).maxAmmo;

						PlaySound(content.reloadSound);
					}

					Respawn respawn;
					respawn.type = Respawn::Item;
					respawn.respawnTime = GetTime() + settings.itemSpawnDelay;
					respawn.itemType = item_iter->type;
					respawn.itemBounds = item_iter->bounds;
					respawns.emplace_back(std::move(respawn));

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
					if (player.health <= 0) {
						continue;
					}

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
							if (player.health <= 0) {
								continue;
							}

							if (player_affected.at(player.playerIndex)) {
								continue;
							}

							if (collide(blast_hit, player.bounds)) {
								player_affected.at(player.playerIndex) = true;
								player.health -= weapon_settings.projectileDamage;

								if (player.health <= 0) {
									player.weapon.reset();

									auto shooter = std::find_if(players.begin(), players.end(), [&projectile](const Player& player) { return player.playerIndex == projectile.ownerPlayerIndex; });
									if (player.playerIndex == shooter->playerIndex) {
										shooter->score -= 1;
									}
									else {
										shooter->score += 1;
									}

									Respawn respawn;
									respawn.type = Respawn::Player;
									respawn.respawnTime = GetTime() + settings.respawnTime;
									respawn.playerIndex = player.playerIndex;
									respawns.emplace_back(std::move(respawn));
								}
							}
						}
					}
				}

				if (weapon_settings.shakeOnHit) {
					cameraShake.shake();
				}

				projectile_iter = projectiles.erase(projectile_iter);
			}
			else {
				projectile.bounds.position = end_position;
				projectile.subpixelPosition = end_position;
				++projectile_iter;
			}
		}

		for (auto respawn_iter = respawns.begin(); respawn_iter != respawns.end();) {
			if (GetTime() >= respawn_iter->respawnTime) {
				if (respawn_iter->type == Respawn::Player) {
					auto player_iter = std::find_if(players.begin(), players.end(), [index = respawn_iter->playerIndex](const Player& player) { return player.playerIndex == index; });
					player_iter->bounds.position = findRespawnPosition();
					player_iter->subpixelPosition = player_iter->bounds.position;
					player_iter->health = settings.playerMaxHealth;
				}
				else if (respawn_iter->type == Respawn::Item) {
					Item item(respawn_iter->itemBounds, respawn_iter->itemType);
					items.emplace_back(std::move(item));
				}

				respawn_iter = respawns.erase(respawn_iter);
			}
			else {
				++respawn_iter;
			}
		}
	}

	std::optional<std::vector<int>> checkEndgame() {

		bool finished = false;
		std::vector<std::pair<int, int>> scores;

		for (const Player& player : players) {
			scores.emplace_back(player.score, player.playerIndex);
			if (player.score == settings.scoreForWin) {
				finished = true;
			}
		}

		if (!finished) {
			return std::nullopt;
		}

		std::vector<int> rankings;

		std::sort(scores.begin(), scores.end());
		for (auto iter = scores.rbegin(); iter != scores.rend(); ++iter) {
			rankings.push_back(iter->second);
		}

		return rankings;
	}

	void renderScene() {
		if (level.textureDirty) {
			level.refreshTexture();
			level.textureDirty = false;
		}

		DrawTexture(level.texture, 0, 0, WHITE);

		for (const Player& player : players) {
			if (player.health <= 0) {
				continue;
			}

			DrawTexture(content.drone, player.bounds.position.x, player.bounds.position.y, settings.playerTints.at(player.playerIndex));
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
			static const std::array<int, 4> offsets{1, 18, 34, 51};

			const int offset = offsets.at(player.playerIndex);
			const Color tint = settings.playerTints.at(player.playerIndex);

			{
				const int score_pixels = int(float(player.score) / float(settings.scoreForWin) * 6);
				for (int i = 0; i < score_pixels; ++i) {
					DrawTexture(content.pixel, offset + 0, 62 - i, tint);
					DrawTexture(content.pixel, offset + 1, 62 - i, tint);
				}
			}

			{
				DrawTexture(content.drone, offset + 3, 57, tint);

				const int health_pixels = int(std::ceil(player.health / settings.playerMaxHealth * 4));
				for (int i = 0; i < health_pixels; ++i) {
					DrawTexture(content.pixel, offset + 3 + i, 62, tint);
				}
			}

			if (player.weapon.has_value()) {
				if (player.weapon == WeaponType::MachineGun) {
					DrawTexture(content.machinegun, offset + 8, 57, WHITE);
				}
				else if (player.weapon == WeaponType::Shotgun) {
					DrawTexture(content.laser, offset + 8, 57, WHITE);
				}
				else if (player.weapon == WeaponType::RocketLauncher) {
					DrawTexture(content.rocketlauncher, offset + 8, 57, WHITE);
				}

				const int ammo_pixels = int(std::ceil(float(player.ammo) / float(settings.weapons.at(*player.weapon).maxAmmo) * 4));
				for (int i = 0; i < ammo_pixels; ++i) {
					DrawTexture(content.pixel, offset + 8 + i, 62, tint);
				}
			}
		}
	}
};

class Menu {
public:

	enum MenuPage {
		Splash,
		SelectPlayers,
		Help1,
		Help2,
		Credits,
		GameStarting,
		Rankings,
	};

	const Settings& settings;
	Content& content;
	const Camera2D& camera;
	MenuPage currentPage;
	int players = 1;
	int bots = 3;
	std::vector<int> rankings;

	Menu(const Settings& _settings, Content& _content, const Camera2D& _camera) : settings(_settings), content(_content), camera(_camera), currentPage(MenuPage::Splash) {
	}

	Menu(const Settings& _settings, Content& _content, const Camera2D& _camera, const std::vector<int>& _rankings) : settings(_settings), content(_content), camera(_camera), currentPage(MenuPage::Rankings), rankings(_rankings) {
	}

	void updateAndRender()
	{
		if (!content.menuVideo.empty()) {
			const int frame = int(GetTime() * 30) % content.menuVideo.size();
			DrawTexture(content.menuVideo.at(frame), 0, 0, Color{ 170, 170, 170, 255 });
		}

		if (currentPage == MenuPage::Splash) {
			DrawTexture(content.splash, 0, 0, WHITE);

			if (button(content.button_credits, 54, 34)) {
				currentPage = MenuPage::Credits;
			}

			if (button(content.button_help, 54, 44)) {
				currentPage = MenuPage::Help1;
			}

			if (button(content.button_play, 54, 54)) {
				currentPage = MenuPage::SelectPlayers;
			}
		}
		else if (currentPage == MenuPage::Help1) {
			DrawTexture(content.help1, 0, 0, WHITE);

			if (button(content.button_play, 1, 54)) {
				currentPage = MenuPage::Help2;
			}
		}
		else if (currentPage == MenuPage::Help2) {
			DrawTexture(content.help2, 0, 0, WHITE);

			if (button(content.button_play, 1, 54)) {
				currentPage = MenuPage::Splash;
			}
		}
		else if (currentPage == MenuPage::Credits) {
			DrawTexture(content.credits, 0, 0, WHITE);

			if (button(content.button_back, 54, 54)) {
				currentPage = MenuPage::Splash;
			}
		}
		else if (currentPage == MenuPage::SelectPlayers) {
			DrawTexture(content.select_players, 0, 0, WHITE);

			{
				if (button(content.button_one, 7, 15, players == 1)) {
					players = 1;
				}

				if (button(content.button_two, 21, 15, players == 2)) {
					players = 2;
				}

				if (button(content.button_three, 35, 15, players == 3)) {
					players = 3;
				}

				if (button(content.button_four, 49, 15, players == 4)) {
					players = 4;
				}
			}

			{
				if (button(content.button_zero, 7, 41, bots == 0)) {
					bots = 0;
				}

				if (button(content.button_one, 21, 41, bots == 1)) {
					bots = 1;
				}

				if (button(content.button_two, 35, 41, bots == 2)) {
					bots = 2;
				}

				if (button(content.button_three, 49, 41, bots == 3)) {
					bots = 3;
				}
			}

			bots = std::clamp(bots, 0, 4 - players);

			if (button(content.button_back, 1, 54)) {
				currentPage = MenuPage::Splash;
			}

			if (button(content.button_play, 54, 54)) {
				currentPage = MenuPage::GameStarting;
			}
		}
		else if (currentPage == MenuPage::Rankings) {
			DrawTexture(content.rankings, 0, 0, WHITE);

			if (rankings.size() >= 1) {
				DrawTexture(content.drone, 36, 23, settings.playerTints.at(rankings.at(0)));
			}

			if (rankings.size() >= 2) {
				DrawTexture(content.drone, 36, 31, settings.playerTints.at(rankings.at(1)));
			}

			if (rankings.size() >= 3) {
				DrawTexture(content.drone, 36, 39, settings.playerTints.at(rankings.at(2)));
			}

			if (rankings.size() >= 4) {
				DrawTexture(content.drone, 36, 47, settings.playerTints.at(rankings.at(3)));
			}

			if (button(content.button_back, 1, 54)) {
				currentPage = MenuPage::Splash;
			}
		}
	}

private:
	bool button(Texture texture, const int x, const int y, const bool selected = false) {
		const Vector2 ray_mousepos = GetScreenToWorld2D(GetMousePosition(), camera);
		const glm::ivec2 mouse_position(ray_mousepos.x, ray_mousepos.y);
		const glm::ivec2 image_position(x, y);
		const glm::ivec2 image_size(texture.width, texture.height);
		const bool mouse_hover = mouse_position.x >= image_position.x && mouse_position.y >= image_position.y &&
			mouse_position.x < image_position.x + image_size.x && mouse_position.y < image_position.y + image_size.y;

		const Color color = mouse_hover ? YELLOW : (selected ? GREEN : WHITE);
		DrawTexture(texture, image_position.x, image_position.y, color);

		if (mouse_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			PlaySound(content.menuSound);
			return true;
		}
		else {
			return false;
		}
	}
};

int main() {
	InitWindow(1280, 720, "Destructive Drones");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);

	InitAudioDevice();

	Camera2D camera;
	memset(&camera, 0, sizeof(Camera2D));

	Settings settings;
	Content content;
	std::unique_ptr<Menu> menu;
	std::unique_ptr<Level> level;
	std::unique_ptr<Session> session;

	menu.reset(new Menu(settings, content, camera));

	while (!WindowShouldClose()) {
		{
			const float pixels_per_unit = float(std::min(GetScreenWidth(), GetScreenHeight())) / 64.0f;
			camera.zoom = pixels_per_unit;
			camera.offset.x = (float(GetScreenWidth()) - pixels_per_unit * 64.0f) / 2.0f;
			camera.offset.y = (float(GetScreenHeight()) - pixels_per_unit * 64.0f) / 2.0f;
		}

		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);

		if (menu) {
			menu->updateAndRender();

			if (menu->currentPage == Menu::GameStarting) {

				level.reset(new Level(settings, "map0.csv"));
				session.reset(new Session(settings, content, *level));

				for (int i = 0; i < menu->players; ++i) {
					session->addPlayer(i, false);
				}

				for (int i = 0; i < menu->bots; ++i) {
					session->addPlayer(menu->players + i, true);
				}

				menu.reset();
			}
		}
		else {
			session->update();
			session->cameraShake.updateCamera(camera);
			session->renderScene();
			session->renderUi();

			std::optional<std::vector<int>> rankings = session->checkEndgame();
			if (rankings.has_value()) {
				session.reset();
				level.reset();
				menu.reset(new Menu(settings, content, camera, *rankings));
			}
		}

		EndMode2D();
		EndDrawing();
	}

	CloseAudioDevice();
	CloseWindow();

	return 0;
}
