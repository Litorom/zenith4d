//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include <stdio.h>

using namespace fdm;

// Initialize the DLLMain
initDLL


std::vector<Item*> getItems()
{
	std::vector<Item*> items;
	items.push_back(new ItemBlock());
	items.push_back(new ItemTool());
	// Add more items as needed  
	return items;
}


$hook(void, StateGame, init, StateManager& s)
{
	// Your code that runs at first frame here (it calls when you load into the world)

	original(self, s);
}

$hook(void, Player, update, World* world, double dt, EntityPlayer* entityPlayer)
{
	// Your code that runs every frame here (it only calls when you play in world, because its Player's function)

	original(self, world, dt, entityPlayer);
}

$hook(bool, Player, keyInput, GLFWwindow* window, World* world, int key, int scancode, int action, int mods)
{
	// Your code that runs when Key Input happens (check GLFW Keyboard Input tutorials)|(it only calls when you play in world, because it is a Player function)
	InventorySession inventoryInstance;
	Player player;

	auto items = getItems(); // Call once and keep the vector

	for (auto* rawItem : items) {
		inventoryInstance.inventory = new InventoryGrid(); // Create a new inventory grid for the player
		std::unique_ptr<Item> item(rawItem); // Take ownership
		inventoryInstance.inventory->addItem(item); // Move into inventory
		// item is now empty, inventory owns it
	}

	int inventorySize = inventoryInstance.inventory->getSlotCount();
	printf("Items found: %d\n", inventorySize);

	if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
		// Copied from backpacks mod, code for opening inventory:
		
		self->inventoryManager.primary = &self->playerInventory; // ok i was wrong its playerInventory
		self->shouldResetMouse = true;
		self->inventoryManager.secondary = inventoryInstance.inventory;


		self->inventoryManager.craftingMenu.updateAvailableRecipes();
		self->inventoryManager.updateCraftingMenuBox();

		inventoryInstance.inventory = self->inventoryManager.secondary;
		inventoryInstance.manager = &self->inventoryManager;

		((InventoryGrid*)self->inventoryManager.secondary)->renderPos = glm::ivec2{ 397,50 };
	}


	return original(self, window, world, key, scancode, action, mods);
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	// initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();
}

