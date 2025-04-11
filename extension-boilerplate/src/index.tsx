import { Action, ActionPanel, List } from "@omnicast/api"

type Fruit = {
  emoji: string;
  name: string;
};

// Create an array of fruits with their emoji and names
const fruits: Fruit[] = [
  { emoji: "🍎", name: "Apple" },
  { emoji: "🍊", name: "Orange" },
  { emoji: "🍌", name: "Banana" },
  { emoji: "🍉", name: "Watermelon" },
  { emoji: "🍇", name: "Grapes" },
  { emoji: "🍓", name: "Strawberry" },
  { emoji: "🍍", name: "Pineapple" },
  { emoji: "🥭", name: "Mango" },
  { emoji: "🍑", name: "Peach" },
  { emoji: "🍐", name: "Pear" },
  { emoji: "🥝", name: "Kiwi" },
  { emoji: "🍒", name: "Cherries" },
  { emoji: "🫐", name: "Blueberries" },
  { emoji: "🥥", name: "Coconut" },
  { emoji: "🍋", name: "Lemon" },
  { emoji: "🍈", name: "Melon" },
  { emoji: "🍏", name: "Green Apple" },
  { emoji: "🥑", name: "Avocado" },
  { emoji: "🫒", name: "Olive" },
  { emoji: "🍅", name: "Tomato" }
];

const FruitList = () => {
	const handleCustomCallback = (fruit: Fruit) => {
		console.log('custom callback fired with', fruit);
	}

	console.log('yolo+1');

	return (
		<List>
			{fruits.map(fruit => (
				<List.Item 
					title={fruit.name}
					icon={fruit.emoji}
					key={fruit.name} 
					actions={
						<ActionPanel>
							<Action.CopyToClipboard content={"Copy emoji"} />
							<Action title="Custom callback" onAction={() => handleCustomCallback(fruit)} />
						</ActionPanel>
					}
				/>
			))}
		</List>
	);
}

const ExampleCommand = () => {
	return <FruitList />
}

export default ExampleCommand;
