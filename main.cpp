//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include "4DKeyBinds.h"
using namespace fdm;

// Initialize the DLLMain
initDLL


InventorySession CreativeInventory;
InventoryGrid inventoryGrid;


// Initializes the creative inventory with all items
void initCreativeInventory() {
	inventoryGrid = InventoryGrid(glm::ivec2{ ((Item::blueprints.size() + 7) / 8), 8});
	CreativeInventory.inventory = &inventoryGrid;
	for (auto& j : Item::blueprints.items()) {
		auto newItem = Item::create(j.key(), 0);

		CreativeInventory.inventory->addItem(newItem);
	}
	return;
}

void openCreativeInventory(GLFWwindow* window, int action, int mods)
{
	if (action != GLFW_PRESS) return;
	if (StateGame::instanceObj.world->getType() != World::TYPE_SINGLEPLAYER) return;

	Player& player = StateGame::instanceObj.player;

	// Copied from backpacks mod, code for opening inventory:
	player.inventoryManager.primary = &player.playerInventory;
	player.shouldResetMouse = true;
	player.inventoryManager.secondary = CreativeInventory.inventory;


	player.inventoryManager.craftingMenu.updateAvailableRecipes();
	player.inventoryManager.updateCraftingMenuBox();

	CreativeInventory.inventory = player.inventoryManager.secondary;
	CreativeInventory.manager = &player.inventoryManager;

	((InventoryGrid*)player.inventoryManager.secondary)->renderPos = glm::ivec2{ 397,50 };
}

$hook(bool, InventoryManager, applyTransfer, InventoryManager::TransferAction action, std::unique_ptr<Item>& selectedSlot, std::unique_ptr<Item>& cursorSlot, Inventory* other)
{
	InventoryManager& actualInventoryManager = StateGame::instanceObj.player.inventoryManager;
	if (!actualInventoryManager.isOpen() || actualInventoryManager.secondary != CreativeInventory.inventory)
		return original(self, action, selectedSlot, cursorSlot, other);
	// no need for else since last line has return
	if (other != CreativeInventory.inventory) {
		if (selectedSlot == nullptr)
			return false;
		if (cursorSlot != nullptr) {
			cursorSlot = nullptr;
			return true;
		}
		else {
			cursorSlot = selectedSlot->clone();
			cursorSlot->count = cursorSlot->getStackLimit();
			return true;
		}
	}
	else
		return original(self, action, selectedSlot, cursorSlot, other);
}

$hook(bool, Player, keyInput, GLFWwindow* window, World* world, int key, int scancode, int action, int mods)
{
	if (!KeyBinds::isLoaded())
	{
		if (key == GLFW_KEY_I)
			openCreativeInventory(window, action, mods);
	}
	return original(self, window, world, key, scancode, action, mods);
}
$exec
{
	KeyBinds::addBind("Zenith's Creative", "Open Creative Inventory", glfw::Keys::C, KeyBindsScope::PLAYER, openCreativeInventory);
}

$hook(void, StateGame, init, StateManager& s)
{
	original(self, s);

	static bool loaded = false;
	if (loaded) return;
	loaded = true;

	initCreativeInventory();
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	// Initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();
}
