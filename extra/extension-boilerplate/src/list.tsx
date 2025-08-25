import React from 'react';
import { List, ActionPanel, Action, showToast, Icon } from '@vicinae/api';

export default function SimpleList() {
	return (
		<List searchBarPlaceholder='Search fruits...'>
			<List.Section title={'Fruits'}>
				{fruits.map(fruit => (
					<List.Item 
						key={fruit.emoji} 
						title={fruit.name} 
						icon={fruit.emoji} 
						keywords={fruit.keywords} 
						actions={
							<ActionPanel>
								<Action.CopyToClipboard title="Copy emoji" content={fruit.emoji} />
								<Action title="Custom action" icon={Icon.Cog} onAction={() => showToast({ title: 'Hello from custom action' })} />
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
    keywords: ["red", "crisp", "sweet", "orchard", "healthy"]
  },
  { 
    emoji: "🍊", 
    name: "Orange", 
    keywords: ["citrus", "vitamin C", "juicy", "tangy", "breakfast"]
  },
  { 
    emoji: "🍌", 
    name: "Banana", 
    keywords: ["yellow", "potassium", "smoothie", "energy", "tropical"]
  },
  { 
    emoji: "🍉", 
    name: "Watermelon", 
    keywords: ["summer", "refreshing", "hydrating", "seeds", "picnic"]
  },
  { 
    emoji: "🍇", 
    name: "Grapes", 
    keywords: ["wine", "cluster", "sweet", "purple", "vineyard"]
  },
  { 
    emoji: "🍓", 
    name: "Strawberry", 
    keywords: ["berry", "jam", "dessert", "romantic", "garden"]
  },
  { 
    emoji: "🍍", 
    name: "Pineapple", 
    keywords: ["tropical", "spiky", "Hawaiian", "sweet", "vacation"]
  },
  { 
    emoji: "🥭", 
    name: "Mango", 
    keywords: ["tropical", "creamy", "exotic", "Indian", "smoothie"]
  },
  { 
    emoji: "🍑", 
    name: "Peach", 
    keywords: ["fuzzy", "summer", "pit", "Georgia", "cobbler"]
  },
  { 
    emoji: "🍐", 
    name: "Pear", 
    keywords: ["teardrop", "autumn", "crisp", "Bartlett", "elegant"]
  },
  { 
    emoji: "🥝", 
    name: "Kiwi", 
    keywords: ["fuzzy", "green", "tangy", "New Zealand", "exotic"]
  },
  { 
    emoji: "🍒", 
    name: "Cherries", 
    keywords: ["red", "pit", "pie", "sweet", "Michigan"]
  },
  { 
    emoji: "🫐", 
    name: "Blueberries", 
    keywords: ["antioxidants", "pancakes", "muffin", "Maine", "superfood"]
  },
  { 
    emoji: "🥥", 
    name: "Coconut", 
    keywords: ["tropical", "milk", "hard shell", "palm tree", "island"]
  },
  { 
    emoji: "🍋", 
    name: "Lemon", 
    keywords: ["sour", "citrus", "yellow", "zest", "cooking"]
  },
  { 
    emoji: "🍈", 
    name: "Melon", 
    keywords: ["cantaloupe", "orange", "sweet", "breakfast", "honeydew"]
  },
  { 
    emoji: "🍏", 
    name: "Green Apple", 
    keywords: ["tart", "Granny Smith", "baking", "crisp", "sour"]
  }
];
