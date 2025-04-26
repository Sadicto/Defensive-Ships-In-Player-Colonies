#pragma once

#include <Spore\BasicIncludes.h>

using namespace Simulator;

#define cPlayerDefensiveShipsManagerPtr intrusive_ptr<cPlayerDefensiveShipsManager>

///
/// In your dllmain Initialize method, add the system like this:
/// ModAPI::AddSimulatorStrategy(new cPlayerDefensiveShipsManager(), cPlayerDefensiveShipsManager::NOUN_ID);
///

class cPlayerDefensiveShipsManager
	: public Simulator::cStrategy,
	App::IMessageListener
{
public:
	struct planetReinforcementStruct {
		cPlanetRecordPtr planet;
		uint32_t timeToReinforce;
	};
	static const uint32_t TYPE = id("Defensive_Ships_In_Player_Colonies::cPlayerDefensiveShipsManager");
	static const uint32_t NOUN_ID = TYPE;

	int AddRef() override;
	int Release() override;
	void Initialize() override;
	void Dispose() override;
	const char* GetName() const override;
	bool Write(Simulator::ISerializerStream* stream) override;
	bool Read(Simulator::ISerializerStream* stream) override;
	void Update(int deltaTime, int deltaGameTime) override;
	bool WriteToXML(XmlSerializer* xml) override;

	void OnModeEntered(uint32_t previousModeID, uint32_t newModeID) override;

	/// @brief Handles the kMsgCombatantKilled message. 
	/// If the killed combatant belongs to the player empire and was a defender ship, 
	/// this function calls DecreaseDefendersOnPlanet.
	/// @param messageID ID of the received message.
	/// @param msg Pointer to the message payload.
	/// @return true.
	bool HandleMessage(uint32_t messageID, void* msg) override;


	static Simulator::Attribute ATTRIBUTES[];

	static cPlayerDefensiveShipsManager* Get();


	/// @brief Adds a defensive ship for the player on the current planet.
	/// @return Pointer to the newly created spaceship.
	cGameDataUFO* SpawnPlayerDefensiveShip();


	/// @brief Returns the maximum number of defenders allowed based on the planet type: outpost, colony, or homeworld.
	/// @param planet
	/// @return The maximum number of defenders for the specified planet.
	int GetMaxDefenders(cPlanetRecord* planet);

	/// @brief Returns the number of defender ships currently stationed on the given planet.
	/// @param planet
	/// @return The number of defender ships on the specified planet.
	int GetDefendersOnPlanet(cPlanetRecord* planet);

	/// @brief Decreases the number of defender ships on the specified planet by one,
	/// and pushes to planetsToReinforce to ensure that the planet will be reinforced.
	/// @param planet
	void DecreaseDefendersOnPlanet(cPlanetRecord* planet);

	/// @brief Adds a defender ship to the planet at the front of the reinforcement queue.
	/// If the planet has not reached its maximum number of defenders, schedules another reinforcement.
	/// @param planetToReinforce
	void AddDefenderToPlanet(cPlanetRecord* planetToReinforce);


	/// @brief Ensures the specified planet has the required number of defensive ships by spawning them if needed.
	/// @param planet
	void ManagePlanetDefenders(cPlanetRecord* planet);





private:
	static cPlayerDefensiveShipsManager* instance;

	// Defenders in an outpost colony, that is, a colony with T = 0.
	int playerOutpostDefenders;
	// Defenders in an normal colony, that is, a colony with T > 0.
	int playerColonyDefenders;
	// Defenders in the homeworld.
	int playerHomeworldDefenders;
	// Time interval (in seconds) required to spawn a new defensive ship.
	int reinforceTime;
	// Queue of planets awaiting reinforcements; planetsToReinforce.front() is the next to reinforce.
	queue<planetReinforcementStruct> planetsToReinforce;
	// Maps each planet to its current number of defensive ships; if a planet is not present, it has the maximum number of defenders.
	eastl::map<cPlanetRecordPtr, int> planetDefenderShips;

	uint32_t elapsedTime;

};