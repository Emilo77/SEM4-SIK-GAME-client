#ifndef ZADANIE02_CLIENT_GAMEINFO_H
#define ZADANIE02_CLIENT_GAMEINFO_H

#include <string>
#include <set>
#include <map>
#include "Utils.h"
#include "Messages.h"
#include <iostream>

class GameInfo {
private:
	enum GameState game_state{LobbyState};
	std::string server_name{};
	uint8_t players_count{0};
	uint16_t size_x{0};
	uint16_t size_y{0};
	uint16_t game_length{0};
	uint16_t explosion_radius{0};
	uint16_t bomb_timer{0};
	std::map<player_id_t, Player> players;

	//GameplayState state
	uint16_t turn{0};
	std::map<player_id_t, Position> player_positions;
	std::vector<Position> blocks;
	std::vector<Bomb> bombs;
	std::vector<Position> explosions;
	std::map<player_id_t, score_t> scores;

	void restart_info();

	void apply_event(Event &event);

	void apply_Hello(struct Hello &message);

	void apply_AcceptedPlayer(struct AcceptedPlayer &message);

	void apply_GameStarted(struct GameStarted &message);

	void apply_Turn(struct Turn &message);

	void apply_GameEnded(struct GameEnded &message);

	void apply_BombPlaced(struct BombPlaced &data);

	void apply_BombExploded(struct BombExploded &data);

	void apply_PlayerMoved(struct PlayerMoved &data);

	void apply_BlockPlaced(struct BlockPlaced &data);


public:

	void apply_changes_from_server(ServerMessageToClient &msg);

	bool is_gameplay() { return game_state == GameplayState; }

	Lobby create_lobby_msg();

	GamePlay create_gameplay_msg();
};


#endif//ZADANIE02_CLIENT_GAMEINFO_H
