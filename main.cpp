//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include "4DKeyBinds.h" // get this header from keybinds repository

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

void openCreativeInventory(Player* self, GLFWwindow* window, int action, int mods)
{
	// Copied from backpacks mod, code for opening inventory:
	self->inventoryManager.primary = &self->playerInventory;
	self->shouldResetMouse = true;
	self->inventoryManager.secondary = CreativeInventory.inventory;


	self->inventoryManager.craftingMenu.updateAvailableRecipes();
	self->inventoryManager.updateCraftingMenuBox();

	CreativeInventory.inventory = self->inventoryManager.secondary;
	CreativeInventory.manager = &self->inventoryManager;

	((InventoryGrid*)self->inventoryManager.secondary)->renderPos = glm::ivec2{ 397,50 };
}

$hook(void,StateGame, init, StateManager& s)
{
	original(self, s);

	static bool loaded = false;
	if (loaded) return;
	loaded = true;

	initCreativeInventory();
}

$hook(bool, InventoryManager, applyTransfer, InventoryManager::TransferAction action, std::unique_ptr<Item>& selectedSlot, std::unique_ptr<Item>& cursorSlot, Inventory* other)
{
	InventoryManager& actualInventoryManager = StateGame::instanceObj.player.inventoryManager;
	if (!actualInventoryManager.isOpen() || actualInventoryManager.secondary != CreativeInventory.inventory)
		return original(self, action, selectedSlot, cursorSlot, other);
	// no need for else since last line has return
	if (other != CreativeInventory.inventory) {
		if (selectedSlot == nullptr)
			return;
		cursorSlot = selectedSlot->clone();
		cursorSlot->count = cursorSlot->getStackLimit();
	}
	else
		return original(self, action, selectedSlot, cursorSlot, other);
}

$hook(bool, Player, keyInput, GLFWwindow* window, World* world, int key, int scancode, int action, int mods)
{
	if (!KeyBinds::isLoaded())
	{
		if (key == GLFW_KEY_C && action == GLFW_PRESS)
			openCreativeInventory(self, window, action, mods);
	}
	return original(self, window, world, key, scancode, action, mods);
}
$exec
{
	KeyBinds::addBind("YourModName", "YourKeybindName", glfw::Keys::C, KeyBindsScope::PLAYER, openCreativeInventory);
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	// Initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();
}
