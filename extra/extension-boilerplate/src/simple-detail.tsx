import { Action, ActionPanel, Detail, showToast } from "@vicinae/api";

const intro = `# Hello world

Your extension is *working* successfully.

Now you can start making changes to this command source file and see it update live here.

If you are online, you should be able to see the Vicinae logo below:

![](https://github.com/vicinaehq/vicinae/raw/main/.github/assets/vicinae-banner.png?raycast-height=100)

---

# Markdown formatting
`;

const headings = `
# Heading 1
## Heading 2
### Heading 3
#### Heading 4
##### Heading 5
###### Heading 6
`;

const textDecorations = `
This text has **bold**, _italic_, ***bold-italic***, and \`code\`.
`;

const lists = `
- List item 1
- List item 2
- List item 3

1. Ordered list item 1
2. Ordered list item 2
3. Ordered list item 3
`;

const images = `
blue square (data:image/svg+xml;utf8 plain):
<img width="100" height='100' src="data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg'><rect width='100' height='100' fill='blue'/></svg>">

green rectangle (data:image/svg+xml urlencoded):
<img width="200" height='50' src="data:image/svg+xml;utf8,${encodeURIComponent(
	"<svg xmlns='http://www.w3.org/2000/svg'><rect width='200' height='50' fill='green'/></svg>",
)}">

yellow rectangle with text (data:image/svg+xml base64):
<img width="200" height='50' src="data:image/svg+xml;utf8;base64,${Buffer.from(
	`
	<svg xmlns='http://www.w3.org/2000/svg'>
		<rect width='200' height='50' fill='yellow'/>
		<text x='100' y='30' dominant-baseline='middle' text-anchor='middle' font-family='sans-serif' font-size='12'>Text in rect!</text>
	</svg>
`,
).toString("base64")}">

markdown format image (data:image/png;base64 with raycast-* size):

![Logo](data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAMAAAAp4XiDAAAAOVBMVEX///8EBAQVFRUpKSk8PDxISEhUVFRfX19ycnJ+fn6Pj4+enp6qqqrBwcHOzs7Y2Njl5eXw8PD5+fkO8WfbAAAAE3RSTlP/1dja3d/h4+bo6+3w9Pb4+vz+kO1liAAAAaNJREFUeNq9ltt2hCAMRY/DRYNELv//sbWKDGmxtjx0P41r2CSTQRJMJ4o44QcSkypLcQou4pHo1FsxGyQ5xZ2UIdnMpcxRLA+erNE7xpIPQovzqdjWSH7WljyHHd5dPfvUOuZTUVsrGONCbkM6Y1ppU7vimmdbvxbb2GZXN0HFuuOq3SmkwN65lUMqpdJrjRwVqO5Gmg+TSb+mg5cmPtayphqdwJexmHAItqwv1nKsDWa54jCKnclEAHERwqSuYkRDxanhVh2OBITwooiLoFcINs0AWMkQPuMN600U0rqOwRA4m/DGmwTEL1kxJMl4yIe8TIKSutxYfmZZqzkD92Hy7IBsZVobvuPqPkGHxyB14Ym3GaBJwOiQ7ZUZEZC0MFRED6LGDQ951XxKIRjgSeDQhUuZow7AKhWPLkHHt+L+RfFSWW+VgZ8/UOTBv7J/YPzNgRk4lsOHv/+K2e4rNvAiD1wXA5fS4NUnL9jtS25L74IduMZ/3ywSTvixJaXSkjIKdNf41qbxJdn4BtrrUBOfzJ9GhYGBZGDsGR+unke43IxwH8fLHH399UsSAAAAAElFTkSuQmCC?raycast-width=50&raycast-height=50)
`;

const table = `
Table with formatting:

| Name         | Age | Country    |
|--------------|-----|------------|
| Alice        |  28 | **Canada** |
| Bob          |  34 | _Germany_  |
| Charlie      |  25 | \`Brazil\` |


Table with alignment:

| Left Align | Center Align | Right Align |
|:-----------|:------------:|------------:|
| Apple      |   Orange     |        Pear |
| Banana     |   Mango      |      Grape  |
`;

const content = [intro, headings, textDecorations, lists, images, table];

export default function SimpleDetail() {
	return (
		<Detail
			markdown={content.map((c) => c.trim()).join("\n\n")}
			actions={
				<ActionPanel>
					<Action
						title="Say hello"
						onAction={() => showToast({ title: "Hello!" })}
					/>
				</ActionPanel>
			}
		/>
	);
}
