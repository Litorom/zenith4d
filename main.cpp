//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include <stdio.h>

using namespace fdm;

// Initialize the DLLMain
initDLL


InventorySession CreativeInventory; 
InventorySession inventoryInstance;
InventoryGrid inventoryGrid;
Player player;

// Initializes the creative inventory with all items
void initCreativeInventory() {
	inventoryGrid = InventoryGrid(glm::ivec2{ (Item::blueprints.size() / 8 ) + 1, 8});
	CreativeInventory.inventory = &inventoryGrid;
	for (auto& j : Item::blueprints.items()) {
		auto newItem = Item::create(j.key(), 4096);

		CreativeInventory.inventory->addItem(newItem);
	}
	return;
}

void replenishCreativeInventory() {
	// TODO
}

$hook(void,StateGame, init, StateManager& s)
{
	original(self, s);

	static bool loaded = false;
	if (loaded) return;
	loaded = true;

	initCreativeInventory();
}

$hook(void, Player, update, World* world, double dt, EntityPlayer* entityPlayer)
{
	// Your code that runs every frame here (it only calls when you play in world, because its Player's function)

	original(self, world, dt, entityPlayer);
}

$hook(bool, Player, keyInput, GLFWwindow* window, World* world, int key, int scancode, int action, int mods)
{
	// Your code that runs when Key Input happens (check GLFW Keyboard Input tutorials)|(it only calls when you play in world, because it is a Player function)
	

	if (key == GLFW_KEY_C && action == GLFW_PRESS) {

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

	return original(self, window, world, key, scancode, action, mods);
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	// Initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();
}

