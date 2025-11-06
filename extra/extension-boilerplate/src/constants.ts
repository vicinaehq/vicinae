export const DATA_URI_SVG_PLAIN =
	"data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg'><rect width='100' height='50' fill='blue'/></svg>";

export const DATA_URI_SVG_URLENCODED = `data:image/svg+xml;utf8,${encodeURIComponent(
	"<svg xmlns='http://www.w3.org/2000/svg'><rect width='100' height='50' fill='green'/></svg>",
)}`;

export const DATA_URI_SVG_BASE64 = `data:image/svg+xml;utf8;base64,${Buffer.from(
	`<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 200 100'>
    <rect width='200' height='100' fill='yellow'/>
    <text x='100' y='55' dominant-baseline='middle' text-anchor='middle' font-family='sans-serif' font-size='20'>Text in rect!</text>
  </svg>`,
).toString("base64")}`;

// Vicinae logo
export const DATA_URI_PNG_BASE64 =
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAMAAAAp4XiDAAAAOVBMVEX///8EBAQVFRUpKSk8PDxISEhUVFRfX19ycnJ+fn6Pj4+enp6qqqrBwcHOzs7Y2Njl5eXw8PD5+fkO8WfbAAAAE3RSTlP/1dja3d/h4+bo6+3w9Pb4+vz+kO1liAAAAaNJREFUeNq9ltt2hCAMRY/DRYNELv//sbWKDGmxtjx0P41r2CSTQRJMJ4o44QcSkypLcQou4pHo1FsxGyQ5xZ2UIdnMpcxRLA+erNE7xpIPQovzqdjWSH7WljyHHd5dPfvUOuZTUVsrGONCbkM6Y1ppU7vimmdbvxbb2GZXN0HFuuOq3SmkwN65lUMqpdJrjRwVqO5Gmg+TSb+mg5cmPtayphqdwJexmHAItqwv1nKsDWa54jCKnclEAHERwqSuYkRDxanhVh2OBITwooiLoFcINs0AWMkQPuMN600U0rqOwRA4m/DGmwTEL1kxJMl4yIe8TIKSutxYfmZZqzkD92Hy7IBsZVobvuPqPkGHxyB14Ym3GaBJwOiQ7ZUZEZC0MFRED6LGDQ951XxKIRjgSeDQhUuZow7AKhWPLkHHt+L+RfFSWW+VgZ8/UOTBv7J/YPzNgRk4lsOHv/+K2e4rNvAiD1wXA5fS4NUnL9jtS25L74IduMZ/3ywSTvixJaXSkjIKdNf41qbxJdn4BtrrUBOfzJ9GhYGBZGDsGR+unke43IxwH8fLHH399UsSAAAAAElFTkSuQmCC";
