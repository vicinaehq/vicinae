import {
	Action,
	ActionPanel,
	Color,
	Icon,
	List,
	showToast,
} from "@vicinae/api";

export default function ListDetail() {
	return (
		<List isShowingDetail searchBarPlaceholder={"Search fruits..."}>
			<List.Section title={"Fruits"}>
				{fruits.map((fruit) => (
					<List.Item
						key={fruit.emoji}
						title={fruit.name}
						icon={fruit.emoji}
						detail={
							<List.Item.Detail
								markdown={fruit.description}
								metadata={
									<List.Item.Detail.Metadata>
										<List.Item.Detail.Metadata.Label
											title="Emoji"
											text={fruit.emoji}
										/>
										<List.Item.Detail.Metadata.TagList title="Properties">
											<List.Item.Detail.Metadata.TagList.Item
												color={fruit.color}
												text="Color"
											/>
										</List.Item.Detail.Metadata.TagList>
										<List.Item.Detail.Metadata.Separator />
										<List.Item.Detail.Metadata.Link
											title={"Emojipedia"}
											target={`https://emojipedia.org/search/?q=${fruit.emoji}`}
											text="Search for emoji"
										/>
									</List.Item.Detail.Metadata>
								}
							/>
						}
						actions={
							<ActionPanel>
								<Action.CopyToClipboard
									title="Copy emoji"
									content={fruit.emoji}
								/>
								<Action
									title="Custom action"
									icon={Icon.Cog}
									onAction={() =>
										showToast({ title: "Hello from custom action" })
									}
								/>
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
	color: Color;
	description: string; // Markdown formatted description
};

export const fruits: Fruit[] = [
	{
		emoji: "🍎",
		name: "Apple",
		color: Color.Red,
		description:
			'# Apple: Nature\'s Perfect Snack\n\n**Apples** (*Malus domestica*) are among the world\'s most popular and culturally significant fruits.\n![](https://images.fineartamerica.com/images/artworkimages/mediumlarge/3/apple-tree-orchard-jane-small.jpg)\nThey belong to the *Rosaceae* family and come in over *7,500 varieties* worldwide.\n\n## Nutritional Profile\n- **Rich in fiber** (4-5g per medium apple), particularly pectin, which aids digestion\n- **Abundant vitamins**: Primarily vitamin C (14% of daily value) and potassium\n- **Antioxidant powerhouse**: Contains quercetin, catechin, and chlorogenic acid that fight free radicals\n- **Low calorie density**: Only about 95 calories per medium fruit\n\n## Historical Significance\nApples have deep cultural roots appearing in:\n- **Biblical stories** (the forbidden fruit in Eden)\n- **Greek mythology** (the golden apples of Hesperides)\n- **Scientific breakthroughs** (Newton\'s gravity inspiration)\n- **Folklore** ("An apple a day keeps the doctor away")\n\n## Culinary Versatility\nApples transform beautifully in the kitchen into:\n* Pies, tarts, and crisps\n* Sauces and butters\n* Ciders (both alcoholic and non-alcoholic)\n* Dried snacks and preserves\n\n> *"Anyone who stops learning is old, whether at twenty or eighty. Anyone who keeps learning stays young."* - Henry Ford (who was known to be an apple enthusiast)',
	},
	{
		emoji: "🍊",
		name: "Orange",
		color: Color.Orange,
		description:
			"# Orange: Citrus Sunshine\n\n**Oranges** (*Citrus × sinensis*) are vibrant citrus fruits originally from Asia but now cultivated worldwide.\n ![](https://mypaintingclub.com/images/lessons/main/110_1250_Oranges.jpg)\nThey're botanically classified as *hesperidia*, a type of modified berry with a leathery rind.\n\n## Nutritional Marvel\n- **Vitamin C champion**: One medium orange provides 91% of daily requirements\n- **Folate powerhouse**: Essential for cell division and DNA synthesis\n- **Hydration helper**: Contains about 87% water\n- **Fiber-rich**: 3g per fruit aids digestive health\n- **Immune system booster**: Contains over 170 different phytochemicals and 60 flavonoids\n\n## Historical Journey\nOranges have fascinating origins:\n- **Ancient roots**: First cultivated in China around 2500 BCE\n- **Name etymology**: From Sanskrit 'naranga' through Persian and Arabic to 'orange'\n- **Columbus connection**: Brought to the Americas during his second voyage in 1493\n- **Florida beginning**: First US groves planted in St. Augustine in the 1500s\n\n## Beyond Eating\nOranges contribute to our lives in many ways:\n* Essential oil production for aromatherapy\n* Zest for flavoring countless dishes\n* Ornamental beauty (especially bitter oranges)\n* D-Limonene in the peel used in cleaning products\n\n> *\"The orange is the intellectual antidote to the spiritual poison of boredom.\"* - Anthony Powell, reflecting on the multi-sensory experience of oranges",
	},
	{
		emoji: "🍌",
		name: "Banana",
		color: Color.Yellow,
		description:
			"# Banana: Nature's Perfect Package\n\n**Bananas** (*Musa* spp.) are elongated, tropical fruits that grow in hanging clusters on plants technically classified as herbs, not trees.\n![](https://www.paintingstar.com/static/gallery/2005/05/27/52977cc454218.jpg?Green+Bananas+Artwork+by+Walt+Kuhn)\nThey are the world's *fourth most valuable food crop*.\n\n## Nutritional Powerhouse\n- **Energy source**: Rich in easily digestible carbohydrates (about 27g per medium banana)\n- **Potassium marvel**: Contains 422mg per fruit (9% DV), supporting heart and muscle function\n- **Vitamin B6 abundance**: 22% of daily needs, supporting brain development and immune function\n- **Mood enhancer**: Contains tryptophan which converts to serotonin in the body\n- **Prebiotic fiber**: Resistant starch feeds beneficial gut bacteria\n\n## Cultivation Fascination\n- **Not grown from seeds**: Commercially propagated through rhizomes\n- **Technically berries**: Botanically classified as berry fruits\n- **Radiation detector**: Slightly radioactive due to naturally occurring potassium-40\n- **Clonal crisis**: Most commercial bananas are genetically identical Cavendish variety, vulnerable to Panama disease\n- **Ripening science**: Produce ethylene gas that speeds their own ripening\n\n## Cultural Significance\nBananas have shaped human society:\n* Foundation of economies in over 130 countries\n* Subject of labor rights movements and geopolitical struggles\n* Featured in religious ceremonies across Southeast Asia\n* Inspiration for comedy, from slapstick to Monty Python\n\n> *\"Time flies like an arrow; fruit flies like a banana.\"* - Groucho Marx, highlighting the universal appeal of this beloved fruit",
	},
	{
		emoji: "🍉",
		name: "Watermelon",
		color: Color.Green,
		description:
			"# Watermelon: Desert Miracle\n\n**Watermelons** (*Citrullus lanatus*) are large, vine-grown fruits with an impressive lineage dating back over 5,000 years to the Kalahari Desert region. They're botanically categorized as 'pepos'—a special type of berry with a thick rind and fleshy center.\n\n## Hydrating Wonder\n- **Nature's canteen**: Contains approximately 92% water, perfect for hydration\n- **Electrolyte balance**: Rich in potassium, magnesium, and trace minerals lost through sweat\n- **Lycopene leader**: Contains more of this cancer-fighting antioxidant than raw tomatoes\n- **Heart healthy**: Contains citrulline, which may improve blood flow and reduce blood pressure\n- **Weight management ally**: High water content creates satiety with minimal calories (only 46 calories per cup)\n\n## Historical Significance\n- **Ancient Egyptian honor**: Seeds found in King Tutankhamun's tomb\n- **Medicinal history**: Prescribed for urinary and kidney problems in ancient medicine\n- **Slavery connection**: African slaves brought seeds to America\n- **Symbol of prosperity**: Featured in paintings throughout history as a luxury item\n- **Japanese luxury**: Premium varieties can sell for thousands of dollars per fruit\n\n## Beyond the Red\nWatermelons come in surprising varieties:\n* Yellow and orange-fleshed cultivars\n* Seedless hybrids (actually containing soft, white, undeveloped seeds)\n* Personal-sized mini watermelons\n* Square watermelons (grown in box molds in Japan)\n\n> *\"When one has tasted watermelon, one knows what angels eat.\"* - Mark Twain, capturing the divine refreshment of this remarkable fruit",
	},
	{
		emoji: "🍇",
		name: "Grapes",
		color: Color.Purple,
		description:
			'# Grapes: Ancient Berries of Civilization\n\n**Grapes** (*Vitis vinifera* and related species) are small, juicy berries growing in clusters on woody vines. With over *8,000 varieties* worldwide, they\'re among the oldest cultivated plants, with evidence of viticulture dating back 8,000 years.\n\n## Nutritional Composition\n- **Antioxidant treasury**: Particularly rich in resveratrol, quercetin, and catechins\n- **Heart protectors**: Flavonoids help reduce LDL cholesterol and prevent clotting\n- **Brain boosters**: Studies show improved cognitive function and reduced risk of dementia\n- **Cancer fighters**: Compounds may inhibit growth of cancer cells\n- **Energy source**: Natural sugars (fructose and glucose) provide quick energy\n\n## Historical Transformation\nGrapes have shaped human civilization through:\n- **Wine production**: Revolutionized trade, religion, and medicine\n- **Agricultural innovation**: Among first fruits deliberately cultivated\n- **Symbolic importance**: Featured in Roman, Greek, and Christian iconography\n- **Economic driver**: Created entire economic systems, from ancient Mediterranean to modern Napa\n- **Preservation revolution**: Drying into raisins extended food availability year-round\n\n## Beyond Consumption\nGrapes contribute to our world in unexpected ways:\n* Grapeseed oil used in culinary and cosmetic applications\n* Vine extracts used in pharmaceutical preparations\n* Grape waste (pomace) used as fertilizer and animal feed\n* Inspiration for art, literature, and architecture throughout history\n\n> *"Men are like wine - some turn to vinegar, but the best improve with age."* - Pope John XXIII, drawing on the timeless wisdom grape cultivation has provided',
	},
	{
		emoji: "🍓",
		name: "Strawberry",
		color: Color.Red,
		description:
			"# Strawberry: Nature's Rubies\n\n**Strawberries** (*Fragaria* × *ananassa*) are unique members of the rose family with their seeds on the outside. What we consider 'berries' are technically 'accessory fruits' where the fleshy part develops from the receptacle rather than the ovary.\n![](https://images.saatchiart.com/saatchi/767571/art/6164377/5234161-HSC00002-7.jpg)\n\n## Nutritional Marvel\n- **Vitamin C powerhouse**: Just 8 medium berries provide 150% of daily needs\n- **Antioxidant abundance**: Contain over 40 different polyphenolic compounds\n- **Anti-inflammatory agents**: Rich in anthocyanins and ellagic acid\n- **Low glycemic index**: Despite sweet taste, minimal blood sugar impact\n- **Fiber contribution**: 3g per cup supporting digestive health\n\n## Botanical Uniqueness\n- **External seeds**: The only fruit with seeds (called achenes) on the exterior\n- **Not true berries**: Botanically classified as 'aggregate accessory fruits'\n- **Wild diversity**: Over 20 wild species exist globally\n- **Modern hybrid**: Commercial varieties result from crossing American and Chilean wild strawberries\n- **Perennial plants**: Can produce for up to 5 years with proper care\n\n## Cultural Significance\nStrawberries have captivated humanity through:\n* Traditional medicinal uses for digestive ailments\n* Rituals in Native American cultures celebrating spring's first fruits\n* Symbolism in medieval art representing perfection and righteousness\n* Cultivation innovations, including modern vertical farming techniques\n* Culinary versatility from desserts to savory applications\n\n> *\"Doubtless God could have made a better berry, but doubtless God never did.\"* - William Butler, 17th century writer, expressing the universal admiration for this remarkable fruit",
	},
	{
		emoji: "🍍",
		name: "Pineapple",
		color: Color.Yellow,
		description:
			"# Pineapple: Crown Jewel of Tropical Fruits\n\n**Pineapples** (*Ananas comosus*) are tropical marvels comprising many individual 'fruitlets' fused around a central core. Despite their name and appearance, they neither contain pine nor are apples—they're bromeliads related to Spanish moss and air plants.\n\n## Nutritional Complexity\n- **Enzymatic powerhouse**: Contains bromelain, a protein-digesting enzyme complex with anti-inflammatory properties\n- **Manganese source**: One cup provides 67% of daily needs, supporting bone formation and metabolism\n- **Vitamin C abundance**: 88% of daily requirements per cup\n- **Digestive aid**: The combination of fiber and bromelain supports healthy digestion\n- **Immune booster**: Rich in vitamin C and zinc, crucial for immune function\n\n## Historical Journey\n- **Indigenous treasure**: Cultivated for at least 2,000 years in South America\n- **Name origin**: European explorers thought they resembled pine cones\n- **Colonial status symbol**: So rare in Europe that they were rented as party centerpieces\n- **Hospitality symbol**: Became emblematic of welcome in Colonial America\n- **Hawaiian controversy**: Introduced in the 1900s, creating an industry that transformed the islands' economy and ecology\n\n## Beyond Consumption\nPineapples contribute to our world beyond their delicious flavor:\n* Bromelain extracted for meat tenderizers and anti-inflammatory medications\n* Fiber from leaves used for luxury textiles like piña cloth\n* Architectural and design motif symbolizing hospitality\n* Pineapple waste used for animal feed and natural fertilizer\n\n> *\"Be a pineapple: Stand tall, wear a crown, and be sweet on the inside.\"* - Katherine Gaskin, capturing the inspirational qualities of this remarkable fruit",
	},
	{
		emoji: "🥭",
		name: "Mango",
		color: Color.Orange,
		description:
			"# Mango: The King of Fruits\n\n**Mangoes** (*Mangifera indica*) are tropical stone fruits with an ancient lineage dating back over 4,000 years in India. With more than *1,000 varieties* cultivated worldwide, they range in size from tiny egg-shaped fruits to massive 5-pound specimens.\n\n## Nutritional Excellence\n- **Vitamin A abundance**: One cup provides 45% of daily needs, supporting vision and immune function\n- **Digestive ally**: Contains amylases and other enzymes that break down complex carbohydrates\n- **Antioxidant rich**: High levels of quercetin, fisetin, astragalin, and gallic acid\n- **Immune support**: 75% of daily vitamin C in one cup\n- **Fiber source**: 2.6g per cup, supporting digestive health and blood sugar balance\n\n## Cultural Significance\n- **Religious importance**: Sacred in Hinduism, often used in religious ceremonies\n- **National symbol**: National fruit of India, Pakistan, and the Philippines\n- **Literary presence**: Mentioned in Sanskrit poetry from 4th century BCE\n- **Linguistic impact**: The word 'mango' originated from Malayalam 'manga'\n- **Trade influence**: So valuable historically that special mango ambassadors existed\n\n## Botanical Wonder\nMangoes possess fascinating characteristics:\n* Related to cashews and pistachios (family Anacardiaceae)\n* Can grow to be 100+ year-old trees reaching 100 feet tall\n* Produce complex secondary compounds that give distinct flavors\n* Demonstrate seasonal flowering influenced by temperature changes\n* Exhibit self-incompatibility, requiring cross-pollination for best fruit set\n\n> *\"The taste of a mango is a delicate balance between heaven and earth.\"* - Indian proverb, reflecting the sacred status this fruit has held for millennia",
	},
	{
		emoji: "🍑",
		name: "Peach",
		color: Color.Red,
		description:
			"# Peach: The Fruit of Immortality\n\n**Peaches** (*Prunus persica*) are stone fruits with velvety skin and sweet, juicy flesh surrounding a single large seed. Despite their name suggesting Persian origins, they originated in Northwest China over 8,000 years ago.\n\n## Nutritional Profile\n- **Skin health booster**: Rich in vitamin A, promoting cell regeneration and skin elasticity\n- **Potassium source**: Contains 293mg per fruit, supporting heart and muscle function\n- **Antioxidant treasury**: Contains chlorogenic acid, catechins, and anthocyanins\n- **Digestive aid**: Soluble and insoluble fiber support gut health (2.6g per medium fruit)\n- **Low glycemic impact**: Despite sweetness, causes minimal blood sugar spikes\n\n## Cultural Significance\n- **Chinese symbolism**: Represents immortality and unity, commonly depicted in art\n- **Linguistic legacy**: The Chinese word for peach (桃, táo) appears in many expressions symbolizing longevity\n- **Literary presence**: Featured prominently from Chinese mythology to Roald Dahl's 'James and the Giant Peach'\n- **Artistic muse**: Inspired paintings from Renaissance still lifes to impressionist masterpieces\n- **Emoji double meaning**: The peach emoji has taken on additional anatomical connotations in modern digital communication\n\n## Botanical Curiosities\nPeaches present fascinating features:\n* Close relatives of almonds, with similar pits\n* Available in freestone and clingstone varieties\n* Exist in white and yellow-fleshed cultivars\n* Demonstrate extraordinary genetic diversity with thousands of varieties\n* Require chilling hours (time below 45°F) to properly fruit\n\n> *\"Life is better than death, I believe, if only because it is less boring, and because it has fresh peaches in it.\"* - Alice Walker, capturing the simple yet profound pleasure peaches bring",
	},
	{
		emoji: "🍐",
		name: "Pear",
		color: Color.Yellow,
		description:
			"# Pear: The Aristocrat of Fruits\n\n**Pears** (*Pyrus* species) are pomaceous fruits with a distinctive bell or teardrop shape. With evidence of cultivation dating back 3,000 years, they're among civilization's oldest cultivated fruits with over *3,000 known varieties* worldwide.\n\n## Nutritional Excellence\n- **Fiber powerhouse**: 6g per medium fruit, both soluble and insoluble forms\n- **Hypoallergenic nature**: Often the first fruit introduced to infants due to low allergenicity\n- **Copper abundance**: 11% of daily needs, supporting nerve function and iron metabolism\n- **Antioxidant treasury**: Contains glutathione, important for cellular detoxification\n- **Anti-inflammatory properties**: Flavonoids help reduce systemic inflammation\n\n## Historical Significance\n- **Literary presence**: Mentioned in Homer's 'Odyssey' as 'gifts from the gods'\n- **Artistic muse**: Featured prominently in Renaissance still life paintings\n- **Royal connections**: Cultivated in the gardens of Louis XIV, who adored the fruit\n- **Western migration**: European varieties brought by settlers transformed American agriculture\n- **Gift tradition**: Asian pear varieties are traditional gifts symbolizing longevity\n\n## Botanical Marvels\nPears exhibit fascinating characteristics:\n* One of few fruits that ripens better off the tree than on it\n* Wood prized for instruments and fine furniture due to stability\n* Extremely long-lived trees, sometimes producing for over 200 years\n* Stone cells (sclereids) create characteristic gritty texture\n* Intricate cultivation requirements, including specific chilling hours\n\n> *\"The trees were planted for the benefit of another age.\"* - Anonymous English grower, reflecting the multi-generational nature of pear cultivation",
	},
	{
		emoji: "🥝",
		name: "Kiwi",
		color: Color.Green,
		description:
			"# Kiwifruit: The Edible Berry from the Orient\n\n**Kiwifruit** (*Actinidia deliciosa* and related species) are fuzzy-skinned, emerald-fleshed berries with tiny black seeds. Originally called 'Chinese gooseberries,' they were renamed 'kiwifruit' by New Zealand marketers in the 1960s to avoid Cold War associations.\n\n## Nutritional Superstar\n- **Vitamin C champion**: Contains more vitamin C per ounce than oranges (85mg per fruit, 94% DV)\n- **Sleep promoter**: Natural source of serotonin, which converts to melatonin\n- **Digestive enzyme**: Contains actinidin, which helps break down proteins\n- **Potassium source**: More potassium per serving than bananas by weight\n- **Fiber-rich**: 2g per fruit, including both soluble and insoluble forms\n\n## Global Journey\n- **Chinese origins**: Native to the Yangtze Valley, where wild varieties still grow\n- **Missionary introduction**: Seeds first brought to New Zealand in 1904\n- **Commercial breakthrough**: First exported commercially in the 1950s\n- **Naming strategy**: Rebranded after New Zealand's national bird\n- **Growth explosion**: From unknown curiosity to global staple in just decades\n\n## Botanical Curiosities\nKiwifruits possess fascinating features:\n* Climbing vines that can grow 30 feet annually\n* Dioecious plants requiring separate male and female vines\n* Available in green, golden (yellow-fleshed), and red varieties\n* Related to over 50 other Actinidia species\n* Naturally contain oxalates, which some individuals must avoid\n\n> *\"Small fruits with big nutrition, kiwi teaches us that appearances can be deceiving.\"* - Nutrition researcher Dr. Paul Lachance, highlighting the unexpected nutritional density of this unassuming fruit",
	},
	{
		emoji: "🍒",
		name: "Cherries",
		color: Color.Red,
		description:
			"# Cherries: Rubies of the Orchard\n\n**Cherries** (*Prunus avium* for sweet and *Prunus cerasus* for sour varieties) are small stone fruits related to plums, peaches, and almonds. With their distinctive stems and bright colors, they've been cultivated for at least 6,000 years.\n\n## Nutritional Profile\n- **Anti-inflammatory powerhouse**: Rich in anthocyanins that reduce pain and inflammation\n- **Melatonin source**: Natural sleep aid and circadian rhythm regulator\n- **Antioxidant abundance**: Contains quercetin, kaempferol, and vitamin C\n- **Potassium provider**: 10% of daily needs in one cup, supporting heart function\n- **Low glycemic impact**: Minimal blood sugar effect despite natural sweetness\n\n## Historical Significance\n- **Roman appreciation**: First cultivation manuals written by Romans, who spread cherries throughout Europe\n- **American legend**: Associated with George Washington in the famous cherry tree story\n- **Japanese symbolism**: Cherry blossoms (sakura) represent the ephemeral nature of life\n- **European tradition**: Associated with good fortune in many countries\n- **Linguistic impact**: 'Cherry-picked' and other expressions have entered common language\n\n## Botanical Fascinations\nCherries present remarkable features:\n* Extremely climate-sensitive, requiring specific chilling hours\n* Brief harvest window of just 2-3 weeks per variety\n* Trees can live and produce for 100+ years\n* Each cherry blossom can produce only one fruit\n* Preservation historically challenging, making fresh cherries a luxury\n\n> *\"Cherries are such a fleeting pleasure, like childhood itself.\"* - Food writer MFK Fisher, capturing the ephemeral joy cherries bring to each summer",
	},
	{
		emoji: "🫐",
		name: "Blueberries",
		color: Color.Blue,
		description:
			"# Blueberries: North America's Superfood Gift\n\n**Blueberries** (*Vaccinium* species) are small, indigo berries with a silvery 'bloom' on their skin. One of the few commercially important fruits native to North America, they've been harvested by indigenous peoples for over 13,000 years.\n\n## Nutritional Excellence\n- **Antioxidant champion**: The highest antioxidant capacity of all commonly consumed fruits\n- **Brain protector**: Studies show improved cognitive function and delayed brain aging\n- **Vision guardian**: Anthocyanins help prevent macular degeneration\n- **Blood sugar regulator**: Improves insulin sensitivity despite natural sweetness\n- **Anti-inflammatory agent**: Reduces markers of systemic inflammation\n\n## Scientific Validation\n- **Research darling**: Among the most studied fruits for health benefits\n- **Memory impact**: Demonstrated improvements in short-term memory in older adults\n- **Heart protection**: Associated with reduced risk of cardiovascular disease\n- **Gut microbiome support**: Prebiotic components feed beneficial bacteria\n- **DNA protection**: Compounds help prevent oxidative DNA damage\n\n## Botanical Diversity\nBlueberries exhibit fascinating characteristics:\n* Exist as highbush, lowbush (wild), and rabbiteye varieties\n* Require acidic soil (pH 4.5-5.5) to thrive\n* Rely on specific pollinators, particularly native bumblebees\n* Can live and produce for 50+ years with proper care\n* Demonstrate extreme cold hardiness, surviving temperatures to -40°F\n\n> *\"The blueberry is a miracle berry, as if plucked from the garden of a fairy tale. Small, sweet, with extraordinary powers to heal and protect.\"* - Dr. Steven Pratt, superfood researcher, reflecting on the remarkable health properties packed into each tiny berry",
	},
	{
		emoji: "🥥",
		name: "Coconut",
		color: Color.Red,
		description:
			"# Coconut: The Tree of Life\n\n**Coconuts** (*Cocos nucifera*) are technically neither nuts nor fruits but 'drupes'—fruits with a hard covering around the seed. A single coconut palm can produce up to 75 fruits per year and live for 80-100 years.\n\n## Nutritional Complexity\n- **Medium-chain triglycerides (MCTs)**: Easily metabolized fats that provide quick energy\n- **Electrolyte balance**: Coconut water contains potassium, magnesium, sodium, and calcium\n- **Fiber abundance**: Coconut meat contains 7g of fiber per cup\n- **Manganese source**: One cup of coconut provides 60% of daily needs\n- **Iron contributor**: 13% of daily requirements per cup of raw coconut meat\n\n## Cultural Significance\n- **Hindu symbolism**: Sacred in religious ceremonies, representing purity and prosperity\n- **Complete utilization**: Every part used—husk for rope, shells for fuel, leaves for roofing, trunks for building\n- **Pacific voyages**: Floating coconuts enabled self-propagation across oceans\n- **Economic importance**: Livelihood for millions in tropical regions\n- **Naval history**: 'Coconut wireless' as emergency radio transmitters in WWII\n\n## Scientific Marvel\nCoconuts possess remarkable features:\n* Self-sustaining propagation through floating\n* Natural water filtration system creating sterile liquid\n* Salt-tolerant growth even in coastal splash zones\n* Extreme durability withstanding hurricanes\n* Genetic diversity allowing adaptation to varied environments\n\n> *\"He who plants coconut trees, plants food and drink, vessels and clothing, a habitation for himself, and a heritage for his children.\"* - South Pacific proverb, capturing the extraordinary versatility of this life-sustaining plant",
	},
	{
		emoji: "🍋",
		name: "Lemon",
		color: Color.Yellow,
		description:
			'# Lemon: The Versatile Citrus\n\n**Lemons** (*Citrus limon*) are bright yellow citrus fruits with highly acidic juice. Despite their tartness, they\'re actually a hybrid, believed to be a cross between bitter orange and citron, first cultivated in Assam, India.\n\n## Biochemical Powerhouse\n- **Vitamin C concentration**: 51mg per fruit (58% DV), crucial for immune function and collagen production\n- **Citric acid abundance**: Makes up 5-8% of the juice, creating the characteristic sour taste\n- **Antioxidant profile**: Rich in hesperidin and d-limonene, supporting cellular health\n- **Potassium content**: 80mg per fruit, supporting heart and muscle function\n- **Pectin source**: Soluble fiber concentrated in the peel, supporting digestive health\n\n## Historical Impact\n- **Scurvy prevention**: Changed maritime history by preventing vitamin C deficiency on long voyages\n- **Medicinal uses**: Documented in Arabic medical texts dating from the 12th century\n- **Economic influence**: Shaped Mediterranean agriculture and global trade routes\n- **Linguistic legacy**: "When life gives you lemons, make lemonade" and other cultural sayings\n- **Culinary revolution**: Transformed preservation techniques and flavor profiles across cuisines\n\n## Scientific Applications\nLemons contribute beyond nutrition:\n* Natural antibacterial and antimicrobial properties\n* Invisible ink applications using the acidic juice\n* Cleaning agent due to acid content and natural fragrance\n* Natural preservative extending food shelf life\n* pH adjustment in numerous chemical processes\n\n> *"A lemon is not a failure of an orange. It is a brilliant achievement in itself."* - Culinary historian Elizabeth David, highlighting the lemon\'s unique place in world cuisine',
	},
	{
		emoji: "🍈",
		name: "Melon",
		color: Color.Yellow,
		description:
			'# Melon: Desert-Born Refreshment\n\n**Melons** (various *Cucumis* species) are vine-grown fruits with hard rinds protecting sweet, juicy flesh. Evolved in the arid regions of Africa and Southwest Asia, their high water content made them invaluable for desert travelers.\n\n## Nutritional Hydration\n- **Water abundance**: Typically 90% water by weight, providing critical hydration\n- **Electrolyte balance**: Rich in potassium, sodium, and magnesium to replace lost minerals\n- **Low caloric density**: Only about 60 calories per cup despite sweet flavor\n- **Vitamin A source**: Yellow and orange varieties provide significant beta-carotene\n- **Lycopene content**: Red-fleshed watermelons contain this powerful antioxidant\n\n## Diverse Family\n- **Cantaloupe**: Netted skin with orange flesh, rich in beta-carotene\n- **Honeydew**: Smooth, pale skin with green flesh, highest in natural sugar content\n- **Casaba**: Wrinkled yellow skin with white flesh, known for long shelf life\n- **Persian**: Similar to cantaloupe but larger, with greener skin\n- **Charentais**: French variety with smooth skin and intensely aromatic flesh\n\n## Cultivation Ingenuity\nMelons demonstrate agricultural innovations:\n* Deliberate hybridization dating back thousands of years\n* Sex-determining genes that can be manipulated for seedless varieties\n* Drought-resistant characteristics through deep root systems\n* Communication with pollinators through volatile organic compounds\n* Selective breeding for transportation durability without sacrificing flavor\n\n> *"A perfect melon should have the sweetness of honey, the juiciness of nectar, the coolness of snow, and the scent of musk."* - Ancient Persian saying, revealing the centuries-old appreciation of this perfect summer fruit',
	},
	{
		emoji: "🍏",
		name: "Green Apple",
		color: Color.Green,
		description:
			"# Green Apple: The Tart Counterpoint\n\n**Green Apples** (typically *Malus domestica* 'Granny Smith' and related varieties) are crisp, acidic counterparts to their sweeter red and yellow cousins. Originally developed in Australia in 1868, they've become essential in both culinary applications and fresh consumption.\n\n## Nutritional Distinction\n- **Lower sugar content**: Typically 80% the sugar of red varieties, with more malic acid\n- **Pectin abundance**: Higher concentration of this soluble fiber than red varieties\n- **Antioxidant profile**: Rich in quercetin and catechin, concentrated in the skin\n- **Malic acid content**: Creates the characteristic tartness while aiding energy production in the body\n- **Stable blood sugar impact**: Lower glycemic impact due to fiber-to-sugar ratio\n\n## Culinary Significance\n- **Baking stability**: Maintains structure and doesn't turn mushy when heated\n- **Flavor balance**: Provides acidity to cut through rich, fatty dishes\n- **Color retention**: Flesh oxidizes (browns) more slowly than other varieties\n- **Traditional pairings**: Classical companion to sharp cheeses, particularly cheddar\n- **Preservation versatility**: Ideal for both sweet and savory preservation methods\n\n## Horticultural Marvel\nGreen apples demonstrate fascinating characteristics:\n* Extreme storage longevity, sometimes keeping 12+ months under proper conditions\n* Consistent bearing patterns with less alternate-year bearing than some varieties\n* Adaptability to varied growing conditions across multiple continents\n* Pronounced ethylene production affecting ripening of other fruits\n* Natural pest resistance through higher acid content\n\n> *\"The crunch of a green apple reminds us that nature sometimes packages its sweetness inside a bold challenge.\"* - Chef Alice Waters, recognizing how the initial tartness gives way to complex sweetness that red apples often lack",
	},
];
