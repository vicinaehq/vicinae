import { Action, ActionPanel, Icon, List, showToast, Toast } from "@vicinae/api";

export default function SimpleList() {
	return (
		<List searchBarPlaceholder="Search fruits...">
			<List.Section title={"Fruits"}>
				{fruits.map((fruit) => (
					<List.Item
						key={fruit.emoji}
						title={fruit.name}
						icon={fruit.emoji}
						keywords={fruit.keywords}
						actions={
							<ActionPanel>
								<Action.CopyToClipboard
									title="Copy emoji"
									content={fruit.emoji}
								/>
								<Action 
									title="test toast" 
									onAction={async () => {
										const toast = await showToast(Toast.Style.Success, 'title', 'selected successfully');
										setTimeout(() => toast.hide(), 500);
									}
								} />
								<Action
									shortcut={{key: 'arrowUp', modifiers: ['shift']}}
									title="Custom action"
									icon={Icon.Cog}
									onAction={() =>
										showToast({ title: "Hello from custom action" })
									}
								/>

								{/* Sub Menu */}
								<ActionPanel.Submenu
									title="More actions"
									icon={Icon.Stars}
									shortcut={{ modifiers: ["shift"], key: "m" }}
									onOpen={() =>
										showToast({ title: `More actions for ${fruit.name}` })
									}
								>
									{/* Sections inside Sub Menu */}
									<ActionPanel.Section title="Recipe ideas">
										{/* Sub Menu inside Section */}
										<ActionPanel.Submenu title="Smoothies" icon={Icon.Hammer}>
											{["Breakfast boost", "Green refresh"].map((recipe) => (
												<Action
													key={recipe}
													title={recipe}
													icon={Icon.Leaf}
													onAction={() =>
														showToast({
															title: `${recipe} smoothie recipe selected with fruit ${fruit.name}`,
														})
													}
												/>
											))}
										</ActionPanel.Submenu>

										{/* Multiple Sub Menus in a Section */}
										<ActionPanel.Submenu title="Snacks" icon={Icon.Box}>
											{["Fruit skewer", "Yogurt parfait"].map((recipe) => (
												<Action
													key={recipe}
													title={recipe}
													icon={Icon.Snowflake}
													onAction={() =>
														showToast({
															title: `${recipe} snack recipe selected with fruit ${fruit.name}`,
														})
													}
												/>
											))}
										</ActionPanel.Submenu>
									</ActionPanel.Section>

									{/* Regular action in a Sub Menu */}
									<ActionPanel.Section title="Fun">
										<Action
											title="Random fun fact"
											icon={Icon.LightBulb}
											onAction={() =>
												showToast({
													title: `Random ${fruit.name} fact: ${randomFact(fruit.name)}`,
												})
											}
										/>
									</ActionPanel.Section>
								</ActionPanel.Submenu>
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}

type Fruit = {
	emoji: string;
	name: string;
	keywords: string[];
};

const fruits: Fruit[] = [
	{
		emoji: "🍎",
		name: "Apple",
		keywords: ["red", "crisp", "sweet", "orchard", "healthy"],
	},
	{
		emoji: "🍊",
		name: "Orange",
		keywords: ["citrus", "vitamin C", "juicy", "tangy", "breakfast"],
	},
	{
		emoji: "🍌",
		name: "Banana",
		keywords: ["yellow", "potassium", "smoothie", "energy", "tropical"],
	},
	{
		emoji: "🍉",
		name: "Watermelon",
		keywords: ["summer", "refreshing", "hydrating", "seeds", "picnic"],
	},
	{
		emoji: "🍇",
		name: "Grapes",
		keywords: ["wine", "cluster", "sweet", "purple", "vineyard"],
	},
	{
		emoji: "🍓",
		name: "Strawberry",
		keywords: ["berry", "jam", "dessert", "romantic", "garden"],
	},
	{
		emoji: "🍍",
		name: "Pineapple",
		keywords: ["tropical", "spiky", "Hawaiian", "sweet", "vacation"],
	},
	{
		emoji: "🥭",
		name: "Mango",
		keywords: ["tropical", "creamy", "exotic", "Indian", "smoothie"],
	},
	{
		emoji: "🍑",
		name: "Peach",
		keywords: ["fuzzy", "summer", "pit", "Georgia", "cobbler"],
	},
	{
		emoji: "🍐",
		name: "Pear",
		keywords: ["teardrop", "autumn", "crisp", "Bartlett", "elegant"],
	},
	{
		emoji: "🥝",
		name: "Kiwi",
		keywords: ["fuzzy", "green", "tangy", "New Zealand", "exotic"],
	},
	{
		emoji: "🍒",
		name: "Cherries",
		keywords: ["red", "pit", "pie", "sweet", "Michigan"],
	},
	{
		emoji: "🫐",
		name: "Blueberries",
		keywords: ["antioxidants", "pancakes", "muffin", "Maine", "superfood"],
	},
	{
		emoji: "🥥",
		name: "Coconut",
		keywords: ["tropical", "milk", "hard shell", "palm tree", "island"],
	},
	{
		emoji: "🍋",
		name: "Lemon",
		keywords: ["sour", "citrus", "yellow", "zest", "cooking"],
	},
	{
		emoji: "🍈",
		name: "Melon",
		keywords: ["cantaloupe", "orange", "sweet", "breakfast", "honeydew"],
	},
	{
		emoji: "🍏",
		name: "Green Apple",
		keywords: ["tart", "Granny Smith", "baking", "crisp", "sour"],
	},
];

const randomFact = (fruitName: string) => {
	const facts = [
		(name: string) => `${name} are tasty!`,
		(name: string) => `People love ${name.toLowerCase()}s.`,
		(name: string) => `${name}s are cool.`,
	];

	return facts[Math.floor(Math.random() * facts.length)](fruitName);
};
