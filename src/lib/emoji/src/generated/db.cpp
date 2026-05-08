// clang-format off

#include "generated/db.hpp"
#include <string_view>
#include <array>

#define GRP(idx) GROUPS[idx]

static constexpr std::array<std::string_view, 9> GROUPS = {
"Smileys & Emotion","People & Body","Animals & Nature","Food & Drink","Travel & Places","Activities","Objects","Symbols","Flags"
};

#define KW(idx) KEYWORDS[idx]

static constexpr std::array<std::string_view, 6251> KEYWORDS = {
"grinning_face","face","smile","happy","joy",":D","grin","smiley","grinning_face_with_big_eyes","haha",":)","funny","mouth","open","smiling","grinning_face_with_smiling_eyes","laugh","like","eye","pleased","beaming_face_with_smiling_eyes","kawaii","grinning","grinning_squinting_face","lol","satisfied","glad","XD","big","closed","eyes","laughing","tightly","grinning_face_with_sweat","hot","sweat","relief","cold","exercise","rolling_on_the_floor_laughing","rolling","floor","rofl","rotfl","face_with_tears_of_joy","cry","tears","weep","happytears","crying","tear","slightly_smiling_face","fine","this","upside_down_face","flipped","silly","sarcasm","melting face","heat","disappear","dissolve","dread","liquid","melt","winking_face","mischievous","secret",";)","flirt","wink","winky","smiling_face_with_smiling_eyes","flushed","crush","embarrassed","shy","^^","blush","proud","smiling_face_with_halo","angel","heaven","halo","innocent","fairy","fantasy","tale","smiling_face_with_hearts","love","affection","valentines","infatuation","hearts","adore","three","smiling_face_with_heart_eyes","heart","shaped","star_struck","starry","excited","eyed","wow","face_blowing_a_kiss","kiss","blow","kissing","throwing","kissing_face","3","duck","kissy","whistling","smiling_face","massage","happiness","outlined","relaxed","white","kissing_face_with_closed_eyes","kissing_face_with_smiling_eyes","whistle","smiling face with tear","sad","pretend","grateful","relieved","touched","face_savoring_food","tongue","yummy","nom","delicious","savouring","goofy","hungry","lick","licking","lips","um","yum","face_with_tongue","prank","childish","playful","cheeky","out","stuck","winking_face_with_tongue","crazy","joke","zany_face","large","one","small","wacky","wild","squinting_face_with_tongue","horrible","taste","money_mouth_face","rich","dollar","money","sign","hugging_face","hug","hands","hugs","face_with_hand_over_mouth","whoops","shock","surprise","blushing","covering","quiet","face with open eyes and hand over mouth","silence","amazement","awe","disbelief","embarrass","gasp","scared","face with peeking eye","frightening","embarrassing","captivated","peep","stare","shushing_face","shhh","finger","hush","shh","shush","thinking_face","hmmm","think","consider","chin","shade","thinker","thumb","saluting face","respect","salute","ok","sunny","troops","yes","zipper_mouth_face","sealed","zipper","zip","face_with_raised_eyebrow","distrust","scepticism","disapproval","suspicious","colbert","mild","rock","skeptic","neutral_face","indifference","meh",":|","neutral","deadpan","faced","straight","expressionless_face","indifferent","-_-","inexpressive","unexpressive","face_without_mouth","blank","mouthless","mute","no","silent","dotted line face","invisible","lonely","isolation","depression","depressed","hide","introvert","face in clouds","shower","steam","dream","absentminded","brain","fog","forgetful","haze","head","impractical","unrealistic","smirking_face","mean","smug","flirting","sexual","smirk","suggestive","unamused_face","bored","straight face","serious","unimpressed","skeptical","dubious","ugh","side_eye","dissatisfied","unhappy","face_with_rolling_eyes","eyeroll","frustrated","roll","grimacing_face","grimace","teeth","awkward","eek","nervous","face exhaling","relieve","tired","sigh","exhale","groan","whisper","lying_face","lie","pinocchio","liar","long","nose","shaking face","dizzy","blurry","earthquake","head shaking horizontally","disapprove","indiffernt","left","head shaking vertically","down","nod","relieved_face","phew","content","whew","pensive_face","upset","dejected","sadface","sorrowful","sleepy_face","rest","nap","bubble","side","sleep","snot","drooling_face","drool","sleeping_face","sleepy","night","zzz","snoring","face with bags under eyes","exhausted","face_with_medical_mask","sick","ill","disease","covid","coronavirus","doctor","medicine","surgical","face_with_thermometer","temperature","thermometer","fever","face_with_head_bandage","injured","clumsy","bandage","hurt","bandaged","injury","nauseated_face","vomit","gross","green","throw up","barf","disgust","disgusted","green face","face_vomiting","puke","spew","up","sneezing_face","gesundheit","sneeze","allergy","achoo","hot_face","feverish","red","sweating","overheated","stroke","cold_face","blue","freezing","frozen","frostbite","icicles","ice","woozy_face","intoxicated","tipsy","wavy","drunk","groggy","uneven","dizzy_face","spent","unconscious","xox","cross","crossed","dead","knocked","spiral eyes","face with spiral eyes","confused","nauseous","nausea","hypnotized","trouble","whoa","exploding_head","shocked","mind","blown","blowing","explosion","mad","cowboy_hat_face","cowgirl","hat","partying_face","celebration","woohoo","birthday","horn","party","disguised face","pretent","brows","glasses","moustache","disguise","incognito","smiling_face_with_sunglasses","cool","summer","beach","sunglass","best","bright","eyewear","friends","mutual","snapchat","sun","weather","nerd_face","nerdy","geek","dork","face_with_monocle","stuffy","wealthy","exploration","inspection","confused_face","huh","weird",":/","nonplussed","puzzled","s","face with diagonal mouth","confuse","disappointed","unsure","worried_face","concern",":(","slightly_frowning_face","frowning","frown","frowning_face","megafrown","face_with_open_mouth","impressed",":O","surprised","sympathy","hushed_face","woo","speechless","stunned","astonished_face","poisoned","amazed","drunk face","gasping","totally","flushed_face","flattered","dazed","shame","wide","pleading_face","begging","mercy","grievance","glossy","puppy","simp","face holding back tears","gratitude","angry","resist","frowning_face_with_open_mouth","aw","what","yawning","anguished_face","pained","fearful_face","terrified","fear","oops","anxious_face_with_sweat","concerned face","rushed","sad_but_relieved_face","eyebrow","crying_face",":'(","loudly_crying_face","sobbing","bawling","sob","face_screaming_in_fear","munch","omg","alone","fearful","home","horror","scream","confounded_face","unwell",":S","quivering","scrunched","persevering_face","helpless","persevere","struggling","disappointed_face","downcast_face_with_sweat","hard","work","weary_face","distraught","wailing","tired_face","whine","fed","yawning_face","yawn","face_with_steam_from_nose","gas","pride","triumph","airing","grievances","look","steaming","won","pouting_face","hate","despise","enraged","grumpy","pout","rage","angry_face","annoyed","anger","face_with_symbols_on_mouth","swearing","cursing","cussing","profanity","expletive","foul","grawlix","over","smiling_face_with_horns","devil","horns","evil","imp","purple","red devil","angry_face_with_horns","demon","goblin","skull","skeleton","creepy","death","body","danger","grey","halloween","monster","poison","skull_and_crossbones","deadly","scary","pirate","pile_of_poo","hankey","shitface","fail","turd","shit","comic","crap","dirt","dog","dung","poop","bad","needs_improvement","clown_face","mock","ogre","mask","japanese_ogre","creature","oni","shrek","japanese_goblin","tengu","ghost","spooky","ghoul","alien","UFO","paul","outer_space","et","extraterrestrial","external","alien_monster","game","arcade","play","invader","retro","space","ufo","video","robot","computer","machine","bot","grinning_cat","animal","cats","grinning_cat_with_smiling_eyes","cat_with_tears_of_joy","smiling_cat_with_heart_eyes","loving cat","cat_with_wry_smile","ironic","smirking","kissing_cat","weary_cat","oh","screaming","crying_cat","sad cat","pouting_cat","see_no_evil_monkey","monkey","nature","blind","forbidden","gesture","ignore","mizaru","not","prohibited","hear_no_evil_monkey","deaf","ears","kikazaru","speak_no_evil_monkey","iwazaru","no speaking","love_letter","email","envelope","mail","note","romance","heart_with_arrow","cupid","lovestruck","heart_with_ribbon","box","chocolate","chocolates","gift","valentine","sparkling_heart","sparkle","sparkly","stars heart","growing_heart","pink","heartpulse","multiple","pulse","triple","beating_heart","alarm","heartbeat","pulsating","wifi","revolving_hearts","two","two_hearts","heart_decoration","purple-square","heart_exclamation","decoration","above","an","as","dot","heavy","mark","ornament","punctuation","broken_heart","sorry","break","heartbreak","breaking","brokenhearted","heart on fire","passionate","enthusiastic","burn","lust","sacred","mending heart","broken heart","wounded","healing","healthier","improving","recovering","recuperating","unbroken","well","red_heart","black","pink heart","orange_heart","yellow_heart","bf","gold","green_heart","nct","blue_heart","brand","light blue heart","baby blue","purple_heart","bts","emoji","brown_heart","coffee","black_heart","dark","wicked","grey heart","silver","monochrome","white_heart","pure","kiss_mark","lipstick","hundred_points","score","perfect","numbers","century","exam","quiz","test","pass","hundred","100","full","keep","symbol","anger_symbol","pop","vein","collision","bomb","explode","bang","boom","impact","spark","star","shoot","magic","circle","animations","transitions","sweat_droplets","water","drip","drops","plewds","splashing","workout","dashing_away","wind","air","fast","shoo","fart","smoke","puff","dash","gust","running","vaping","hole","speech_balloon","words","message","talk","chatting","chat","comment","dialog","text","literals","eye_in_speech_bubble","info","am","i","witness","left_speech_bubble","right_anger_bubble","caption","speech","thinking","balloon","zag","zig","thought_balloon","cloud","bedtime","boring","sleeping","waving_hand","wave","goodbye","solong","farewell","hello","hi","palm","raised_back_of_hand","fingers","raised","backhand","hand_with_fingers_splayed","hand","five","raised_hand","stop","highfive","ban","high","vulcan_salute","spock","star trek","between","middle","part","prosper","ring","split","rightwards hand","offer","right","rightward","leftwards hand","leftward","palm down hand","drop","dismiss","palm up hand","lift","demand","beckon","catch","come","leftwards pushing hand","pressing","rightwards pushing hand","ok_hand","limbs","okay","pinched fingers","size","tiny","che","interrogation","ma","purse","sarcastic","vuoi","pinching_hand","amount","little","victory_hand","ohyeah","peace","victory","quotes","v","crossed_fingers","good","lucky","hopeful","index","luck","hand with index finger and thumb crossed","expensive","snap","love_you_gesture","ily","sign_of_the_horns","evil_eye","sign_of_horns","rock_on","metal","call_me_hand","shaka","phone","backhand_index_pointing_left","direction","point","backhand_index_pointing_right","backhand_index_pointing_up","middle_finger","rude","flipping","bird","dito","extended","fu","medio","middle finger","reversed","backhand_index_pointing_down","index_pointing_up","index pointing at the viewer","you","recruit","thumbs_up","thumbsup","awesome","agree","accept","+1","approve","thumbs_down","thumbsdown","dislike","-1","bury","raised_fist","grasp","clenched","power","pump","punch","oncoming_fist","violence","fist","hit","attack","bro","brofist","bump","facepunch","fisted","left_facing_fist","fistbump","leftwards","right_facing_fist","rightwards","right fist","clapping_hands","praise","applause","congrats","yay","clap","golf","round","raising_hands","hooray","yea","arms","banzai","both","festivus","hallelujah","miracle","person","heart hands","appreciation","support","open_hands","butterfly","jazz","palms_up_together","cupped","prayer","dua","facing","handshake","agreement","shake","deal","meeting","shaking","folded_hands","please","hope","wish","namaste","pray","thank you","thanks","appreciate","ask","bow","pressed","together","writing_hand","lower_left_ballpoint_pen","stationery","write","compose","nail_polish","nail_care","beauty","manicure","fashion","nail","slay","cosmetics","nonchalant","selfie","camera","arm","flexed_biceps","flex","strong","biceps","bicep","feats","flexing","muscle","muscles","strength","mechanical_arm","accessibility","prosthetic","mechanical_leg","leg","kick","limb","foot","stomp","ear","hear","sound","listen","hearing","listening","ear_with_hearing_aid","smell","sniff","smelling","sniffing","stinky","smart","intelligent","organ","anatomical heart","health","cardiology","lungs","breathe","breath","exhalation","inhalation","respiration","tooth","dentist","bone","watch","stalk","peek","see","eyeballs","shifty","single","biting lip","sexy","pain","worry","anxious","uncomfortable","worried","baby","child","boy","girl","toddler","newborn","young","gender-neutral","gender","inclusive","unspecified","man","male","guy","teenager","female","woman","maiden","virgin","virgo","zodiac","adult","men","women","person_blond_hair","hairstyle","blonde","haired","mustache","father","dad","classy","sir","man_beard","bewhiskered","bearded","man beard","facial hair","woman beard","man_red_hair","ginger","redhead","man_curly_hair","man_white_hair","old","elder","man_bald","hairless","hair","girls","lady","yellow","woman_red_hair","person_red_hair","woman_curly_hair","person_curly_hair","woman_white_hair","person_white_hair","woman_bald","person_bald","woman_blond_hair","man_blond_hair","older_person","human","senior","old_man","elderly","grandpa","older","old_woman","grandma","nanna","person_frowning","man_frowning","discouraged","woman_frowning","person_pouting","man_pouting","woman_pouting","person_gesturing_no","decline","denied","halt","x","man_gesturing_no","nope","ng","woman_gesturing_no","person_gesturing_ok","ballerina","man_gesturing_ok","woman_gesturing_ok","person_tipping_hand","information","attendant","bellhop","concierge","desk","flick","help","sassy","man_tipping_hand","woman_tipping_hand","person_raising_hand","question","answering","man_raising_hand","woman_raising_hand","deaf_person","deaf_man","deaf_woman","person_bowing","respectiful","apology","cute","deeply","dogeza","man_bowing","favor","woman_bowing","person_facepalming","exasperation","facepalm","hitting","picard","smh","man_facepalming","woman_facepalming","person_shrugging","regardless","doubt","ignorance","shrug","shruggie","man_shrugging","woman_shrugging","health_worker","hospital","healthcare","md","nurse","physician","professional","therapist","man_health_worker","woman_health_worker","student","learn","education","graduate","pupil","school","man_student","graduation","woman_student","teacher","professor","educator","instructor","man_teacher","woman_teacher","judge","law","court","justice","scales","man_judge","woman_judge","farmer","crops","farm","farming","gardener","rancher","worker","man_farmer","woman_farmer","cook","food","kitchen","culinary","chef","cooking","service","man_cook","woman_cook","mechanic","technician","electrician","plumber","repair","tradesperson","man_mechanic","wrench","woman_mechanic","factory_worker","labor","assembly","industrial","welder","man_factory_worker","woman_factory_worker","office_worker","business","accountant","adviser","analyst","architect","banker","clerk","manager","man_office_worker","businessman","ceo","woman_office_worker","businesswoman","scientist","chemistry","biologist","chemist","engineer","lab","mathematician","physicist","man_scientist","research","woman_scientist","technologist","coder","laptop","software","technology","developer","man_technologist","programmer","blogger","woman_technologist","singer","song","artist","performer","actor","entertainer","music","musician","rocker","rockstar","man_singer","aladdin","bowie","sane","woman_singer","painting","draw","creativity","art","paint","painter","palette","man_artist","woman_artist","pilot","fly","plane","airplane","aviation","aviator","man_pilot","woman_pilot","astronaut","outerspace","moon","planets","rocket","stars","man_astronaut","cosmonaut","woman_astronaut","firefighter","fire","firetruck","man_firefighter","fireman","woman_firefighter","police_officer","cop","policeman","policewoman","man_police_officer","police","legal","enforcement","arrest","911","woman_police_officer","detective","spy","or","private","sleuth","man_detective","crime","woman_detective","guard","protect","british","guardsman","man_guard","uk","gb","royal","woman_guard","guardswoman","ninja","ninjutsu","skills","japanese","fighter","hidden","stealth","construction_worker","build","builder","helmet","safety","add_ci","update_ci","man_construction_worker","wip","construction","woman_construction_worker","person with crown","royalty","monarch","noble","regal","prince","crown","king","princess","blond","queen","tiara","person_wearing_turban","headdress","arab","muslim","sikh","man_wearing_turban","indian","hinduism","arabs","woman_wearing_turban","man_with_skullcap","chinese","asian","cap","gua","mao","pi","woman_with_headscarf","hijab","mantilla","tichel","man_in_tuxedo","couple","marriage","wedding","groom","suit","man in tuxedo","formal","woman in tuxedo","bride_with_veil","bride","man with veil","woman with veil","pregnant_woman","pregnancy","pregnant lady","pregnant man","belly","bloated","pregnant person","breast_feeding","nursing","breastfeeding","infant","milk","mother","woman feeding baby","birth","bottle","man feeding baby","person feeding baby","baby_angel","wings","cherub","putto","santa_claus","festival","xmas","father christmas","activity","nicholas","saint","sinterklaas","mrs_claus","mother christmas","mrs.","santa","mx claus","christmas","mx.","superhero","marvel","hero","heroine","superpower","superpowers","man_superhero","woman_superhero","supervillain","criminal","villain","man_supervillain","woman_supervillain","mage","sorcerer","sorceress","witch","wizard","man_mage","woman_mage","magical","oberon","puck","titania","man_fairy","woman_fairy","vampire","blood","twilight","dracula","undead","man_vampire","woman_vampire","unded","merperson","sea","merboy","mergirl","mermaid","merman","merwoman","triton","ariel","elf","legolas","pointed","man_elf","woman_elf","genie","wishes","djinn","djinni","jinni","man_genie","woman_genie","zombie","walking","man_zombie","walking dead","woman_zombie","troll","mystical","person_getting_massage","relax","massaging","salon","spa","man_getting_massage","woman_getting_massage","person_getting_haircut","barber","cutting","hairdresser","parlor","man_getting_haircut","woman_getting_haircut","person_walking","move","hike","pedestrian","walk","walker","man_walking","feet","steps","woman_walking","person walking facing right","peerson","woman walking facing right","man walking facing right","person_standing","still","stand","man_standing","woman_standing","person_kneeling","respectful","kneel","man_kneeling","woman_kneeling","person kneeling facing right","woman kneeling facing right","worship","man kneeling facing right","person_with_probing_cane","person with white cane facing right","visually impaired","man_with_probing_cane","man with white cane facing right","stick","woman_with_probing_cane","woman with white cane facing right","person_in_motorized_wheelchair","disability","person in motorized wheelchair facing right","man_in_motorized_wheelchair","man in motorized wheelchair facing right","mobility","woman_in_motorized_wheelchair","woman in motorized wheelchair facing right","person_in_manual_wheelchair","person in manual wheelchair facing right","man_in_manual_wheelchair","man in manual wheelchair facing right","woman_in_manual_wheelchair","woman in manual wheelchair facing right","person_running","jogging","marathon","run","runner","man_running","race","racing","woman_running","person running facing right","jog","woman running facing right","man running facing right","woman_dancing","fun","dance","dancer","dress","salsa","man_dancing","disco","man_in_suit_levitating","levitate","hover","jump","hovering","jabsco","walt","people_with_bunny_ears","perform","costume","dancing","partying","wearing","men_with_bunny_ears","bunny","boys","women_with_bunny_ears","people","person_in_steamy_room","hamam","sauna","steambath","man_in_steamy_room","steamroom","woman_in_steamy_room","person_climbing","sport","bouldering","climber","man_climbing","sports","hobby","woman_climbing","person_fencing","fencing","sword","fencer","horse_racing","betting","competition","gambling","jockey","racehorse","skier","winter","snow","ski","snowboarder","snowboard","snowboarding","person_golfing","ball","club","golfer","man_golfing","woman_golfing","person_surfing","surf","surfer","man_surfing","ocean","woman_surfing","person_rowing_boat","paddles","rowboat","vehicle","man_rowing_boat","ship","woman_rowing_boat","person_swimming","pool","swim","swimmer","man_swimming","athlete","woman_swimming","person_bouncing_ball","basketball","player","man_bouncing_ball","woman_bouncing_ball","person_lifting_weights","training","bodybuilder","gym","lifter","weight","weightlifter","man_lifting_weights","woman_lifting_weights","person_biking","bicycle","bike","cyclist","bicyclist","man_biking","hipster","woman_biking","person_mountain_biking","biker","man_mountain_biking","transportation","woman_mountain_biking","person_cartwheeling","gymnastic","cartwheel","doing","gymnast","gymnastics","man_cartwheeling","woman_cartwheeling","people_wrestling","wrestle","wrestler","wrestlers","men_wrestling","women_wrestling","person_playing_water_polo","man_playing_water_polo","woman_playing_water_polo","person_playing_handball","man_playing_handball","woman_playing_handball","person_juggling","performance","balance","juggle","juggler","multitask","skill","man_juggling","woman_juggling","person_in_lotus_position","meditate","meditation","serenity","yoga","man_in_lotus_position","zen","mindfulness","woman_in_lotus_position","person_taking_bath","clean","bathroom","bathing","bathtub","person_in_bed","bed","accommodation","hotel","people_holding_hands","friendship","date","hold","nonconforming","women_holding_hands","pair","lesbian","lgbt","woman_and_man_holding_hands","dating","heterosexual","men_holding_hands","bromance","gay","couplekiss","kiss_woman_man","kiss_man_man","kiss_woman_woman","couple_with_heart","loving","couple_with_heart_woman_man","couple_with_heart_man_man","couple_with_heart_woman_woman","family_man_woman_boy","son","family_man_woman_girl","parents","daughter","family_man_woman_girl_boy","children","family_man_woman_boy_boy","sons","family_man_woman_girl_girl","daughters","family_man_man_boy","fathers","family_man_man_girl","family_man_man_girl_boy","family_man_man_boy_boy","family_man_man_girl_girl","family_woman_woman_boy","mothers","family_woman_woman_girl","family_woman_woman_girl_boy","family_woman_woman_boy_boy","family_woman_woman_girl_girl","family_man_boy","parent","family_man_boy_boy","family_man_girl","family_man_girl_boy","family_man_girl_girl","family_woman_boy","family_woman_boy_boy","family_woman_girl","family_woman_girl_boy","family_woman_girl_girl","speaking_head","user","sing","say","mansplaining","shout","shouting","silhouette","speak","bust_in_silhouette","shadow","busts_in_silhouette","group","team","bust","shadows","silhouettes","users","contributors","people hugging","care","family","mom","family adult, adult, child","kid","family adult, adult, child, child","family adult, child","family adult, child, child","footprints","tracking","clothing","footprint","footsteps","print","tracks","fingerprint","monkey_face","circus","banana","gorilla","orangutan","ape","dog_face","friend","woof","pet","faithful","doge","dog2","doggo","guide_dog","seeing","service_dog","assistance","poodle","101","miniature","standard","toy","wolf","fox","raccoon","curious","sly","cat_face","meow","kitten","kitty","cat","cat2","domestic","feline","housecat","black cat","superstition","unlucky","lion","leo","tiger_face","roar","tiger","bengal","tiger2","leopard","african","jaguar","horse_face","brown","moose","canada","sweden","sven","donkey","eeyore","mule","horse","gamble","equestrian","galloping","speed","unicorn","zebra","stripes","safari","stripe","deer","venison","buck","reindeer","stag","bison","ox","buffalo","herd","wisent","cow_face","beef","moo","cow","bull","bullock","oxen","steer","taurus","water_buffalo","cow2","dairy","pig_face","oink","pig","hog","pig2","sow","boar","warthog","pig_nose","snout","ram","sheep","aries","ewe","wool","shipit","lamb","goat","capricorn","camel","desert","hump","arabian","dromedary","two_hump_camel","bactrian","llama","alpaca","guanaco","vicuña","giraffe","spots","elephant","th","mammoth","tusks","extinct","extinction","tusk","woolly","rhinoceros","rhino","hippopotamus","hippo","mouse_face","cheese_wedge","rodent","mouse","dormouse","mice","mouse2","rat","hamster","rabbit_face","spring","easter","rabbit","rabbit2","chipmunk","squirrel","beaver","dam","hedgehog","spiny","bat","batman","bear","teddy","polar bear","arctic","koala","marsupial","panda","sloth","lazy","slow","otter","fishing","skunk","smelly","stink","kangaroo","australia","joey","hop","roo","badger","honey","pester","paw_prints","turkey","thanksgiving","chicken","cluck","hen","rooster","cock","cockerel","hatching_chick","egg","born","baby_chick","front_facing_baby_chick","hatched","standing","tweet","penguin","dove","eagle","bald","mallard","swan","cygnet","duckling","ugly","owl","hoot","wise","dodo","mauritius","obsolete","feather","flight","light","plumage","flamingo","flamboyant","tropical","peacock","peahen","ostentatious","parrot","wing","birds","flying","black bird","crow","goose","jemima","goosebumps","honk","phoenix","immortal","mythtical","reborn","frog","croak","toad","crocodile","reptile","lizard","alligator","croc","turtle","tortoise","terrapin","gecko","snake","hiss","python","bearer","ophiuchus","serpent","dragon_face","myth","dragon","sauropod","dinosaur","brachiosaurus","brontosaurus","diplodocus","t_rex","tyrannosaurus","trex","spouting_whale","whale","whale2","dolphin","fish","flipper","fins","seal","freshwater","pisces","tropical_fish","nemo","blowfish","fugu","pufferfish","shark","jaws","great","octopus","spiral_shell","seashell","coral","reef","jellyfish","sting","tentacles","crab","crustacean","cancer","lobster","bisque","claws","seafood","shrimp","prawn","shellfish","squid","molusc","oyster","diving","pearl","snail","shell","garden","slug","insect","caterpillar","pretty","bug","worm","ant","honeybee","bee","bumblebee","beetle","lady_beetle","ladybug","ladybird","cricket","chirp","grasshopper","orthoptera","cockroach","pests","pest","roach","spider","arachnid","spider_web","silk","cobweb","spiderweb","scorpion","scorpio","scorpius","mosquito","malaria","virus","maggot","rotting","annelid","earthworm","parasite","microbe","amoeba","bacteria","germs","cell","germ","microorganism","bouquet","flowers","flower","plant","cherry_blossom","sakura","white_flower","blossom","cherry","doily","done","paper","stamp","lotus","calm","buddhism","india","purity","vietnam","rosette","military","rose","wilted_flower","drooping","hibiscus","vegetable","sunflower","fall","blossoming flower","daisy","tulip","hyacinth","lavender","seedling","grass","lawn","sprout","sprouting","seed","potted plant","greenery","house","grow","houseplant","nurturing","useless","evergreen_tree","fir","pine","wood","deciduous_tree","rounded","shedding","palm_tree","mojito","coconut","cactus","sheaf_of_rice","crop","grain","wheat","herb","weed","leaf","shamrock","irish","clover","trefoil","four_leaf_clover","ireland","maple_leaf","ca","canadian","falling","fallen_leaf","leaves","autumn","leaf_fluttering_in_wind","tree","flutter","empty nest","nesting","nest with eggs","mushroom","fungus","shroom","toadstool","leafless tree","grapes","fruit","wine","grape","melon","cantaloupe","honeydew","muskmelon","watermelon","picnic","tangerine","orange","mandarin","lemon","citrus","lemonade","lime","acidic","citric","plantain","pineapple","mango","red_apple","mac","green_apple","golden","granny","smith","pear","peach","bottom","butt","cherries","berries","strawberry","berry","blueberries","bilberry","blueberry","kiwi_fruit","gooseberry","kiwifruit","tomato","olive","olives","cocoanut","colada","piña","avocado","eggplant","aubergine","phallic","potato","tuber","vegatable","starch","baked","idaho","carrot","ear_of_corn","cob","maize","maze","hot_pepper","spicy","chilli","chili","bell pepper","capsicum","cucumber","pickle","gherkin","leafy_green","bok choy","cabbage","kale","lettuce","cos","greens","romaine","broccoli","garlic","spice","flavoring","onion","peanuts","nut","nuts","peanut","beans","kidney","legume","chestnut","acorn","ginger root","gingerbread","pea pod","cozy","brown mushroom","root vegetable","radish","bread","breakfast","toast","loaf","croissant","french","crescent","baguette_bread","france","bakery","flatbread","flour","arepa","flat","lavash","naan","pita","pretzel","twisted","germany","soft","twist","bagel","schmear","jewish_bakery","cheese","cream","pancakes","flapjacks","hotcakes","brunch","crêpe","crêpes","hotcake","pancake","waffle","indecisive","iron","chadder","swiss","meat_on_bone","drumstick","barbecue","bbq","manga","poultry_leg","meat","cut_of_meat","cut","chop","lambchop","porkchop","steak","bacon","pork","rashers","hamburger","fast food","cheeseburger","mcdonalds","burger king","french_fries","chips","snack","mcdonald's","pizza","italy","pepperoni","slice","hot_dog","frankfurter","america","hotdog","redhot","sausage","wiener","sandwich","lunch","deli","vegetables","taco","mexican","burrito","wrap","tamale","masa","tamal","wrapped","stuffed_flatbread","stuffed","gyro","mediterranean","doner","falafel","kebab","shawarma","chickpea","falfel","meatball","easter_egg","skillet","fried","frying","pan","shallow_pan_of_food","casserole","paella","curry","pot_of_food","soup","hot pot","bowl","stew","fondue","pot","melted","bowl_with_spoon","cereal","oatmeal","porridge","congee","tableware","green_salad","healthy","popcorn","movie theater","films","drama","corn","popping","butter","salt","condiment","shaker","canned_food","tomatoes","can","preserve","tin","tinned","bento_box","assets","rice_cracker","senbei","rice_ball","onigiri","omusubi","cooked_rice","boiled","steamed","curry_rice","steaming_bowl","noodle","chopsticks","ramen","noodles","spaghetti","italian","pasta","roasted_sweet_potato","goguma","yam","oden","skewer","sushi","rice","sashimi","fried_shrimp","appetizer","tempura","fish_cake_with_swirl","japan","narutomaki","swirl","kamaboko","surimi","design","fishcake","pastry","moon_cake","dessert","mooncake","yuèbǐng","dango","sweet","balls","dumpling","empanada","pierogi","potsticker","gyoza","gyōza","jiaozi","fortune_cookie","prophecy","takeout_box","leftovers","container","pail","take","soft_ice_cream","icecream","mr.","serve","whippy","shaved_ice","cone","ice_cream","doughnut","donut","cookie","oreo","biscuit","chip","birthday_cake","cake","candles","shortcake","piece","cupcake","pie","filling","chocolate_bar","candy","lolly","lollipop","lollypop","sucker","custard","pudding","flan","caramel","creme","honey_pot","bees","honeypot","baby_bottle","drink","feeding","glass_of_milk","beverage","hot_beverage","caffeine","latte","espresso","mug","cafe","tea","teapot","kettle","teacup_without_handle","cup","matcha","sake","alcohol","booze","bar","bottle_with_popping_cork","bubbly","champagne","sparkling","wine_glass","cocktail_glass","martini","tropical_drink","cocktail","tiki","vacation","beer_mug","pub","stein","clinking_beer_mugs","beers","cheers","clink","drinks","clinking_glasses","celebrate","glass","tumbler_glass","liquor","bourbon","scotch","whisky","shot","rum","whiskey","pouring liquid","empty","spill","cup_with_straw","soda","go","juice","malt","milkshake","smoothie","bubble tea","taiwan","boba","milk tea","straw","momi","tapioca","beverage_box","mate","bombilla","chimarrão","cimarrón","maté","yerba","cube","iceberg","hashi","jeotgarak","kuaizi","fork_and_knife_with_plate","eat","meal","dinner","restaurant","cutlery","dining","fork_and_knife","silverware","spoon","kitchen_knife","knife","blade","weapon","butchers","hocho","tool","jar","sauce","store","amphora","vase","aquarius","jug","globe_showing_europe_africa","globe","world","earth","international","planet","globe_showing_americas","USA","globe_showing_asia_australia","east","globe_with_meridians","internet","interweb","i18n","global","web","www","internationalization","localization","world_map","location","travel","map_of_japan","nation","country","asia","compass","magnetic","navigation","orienteering","snow_capped_mountain","photo","environment","mountain","volcano","disaster","eruption","mount_fuji","capped","san","camping","outdoors","tent","campsite","beach_with_umbrella","sand","warm","saharah","desert_island","national_park","stadium","place","concert","venue","grandstand","classical_building","culture","history","building_construction","working","progress","crane","architectural","brick","bricks","clay","mortar","wall","infrastructure","stone","boulder","solid","timber","trunk","log","lumber","hut","structure","roundhouse","yurt","houses","buildings","building","derelict_house","abandon","evict","broken","abandoned","haunted","house_with_garden","office_building","bureau","city","rise","japanese_post_office","communication","postal","post_office","european","surgery","emergency","medical","room","bank","sales","cash","enterprise","bakkureru","bk","branch","accomodation","checkin","h","love_hotel","convenience_store","shopping","groceries","corner","e","eleven®","hour","kwik","mart","shop","teach","clock","elementary","tower","department_store","mall","center","shops","factory","industry","pollution","smog","japanese_castle","fortress","castle","turrets","chapel","church","tokyo_tower","eiffel","statue_of_liberty","american","newyork","new","york","religion","christ","christian","mosque","islam","minaret","domed","roof","hindu_temple","synagogue","judaism","temple","jewish","jew","synagog","shinto_shrine","kyoto","kami","michi","kaaba","mecca","fountain","fresh","feature","park","foggy","bridge","fog bridge","karl","under","night_with_stars","evening","downtown","cityscape","night life","urban","skyline","sunrise_over_mountains","view","morning","sunrise","sunset","cityscape_at_dusk","sky","landscape","good morning","dawn","dusk","bridge_at_night","sanfrancisco","gate","hot_springs","bath","hotsprings","onsen","carousel_horse","carnival","entertainment","fairground","merry","playground slide","amusement","ferris_wheel","londoneye","observation","roller_coaster","playground","rollercoaster","theme","barber_pole","style","barber's","haircut","circus_tent","top","locomotive","train","engine","railway","railway_car","carriage","electric","railcar","railroad","tram","trolleybus","wagon","high_speed_train","bullettrain","shinkansen","bullet_train","public","front","diesel","passenger","regular","train2","metro","blue-square","mrt","underground","tube","subway","light_rail","station","platform","monorail","mountain_railway","car","funicular","tram_car","bus","oncoming_bus","bart","electric bus","trolley","minibus","minivan","mover","ambulance","fire_engine","cars","department","truck","police_car","patrol","oncoming_police_car","front of","🚓 cop","taxi","uber","taxicab","oncoming_taxi","automobile","oncoming_automobile","sport_utility_vehicle","campervan","motorhome","recreational","rv","pickup truck","delivery_truck","resources","articulated_lorry","express","semi","tractor","agriculture","racing_car","formula","f1","motorcycle","motorbike","motor_scooter","vespa","sasha","cycle","manual_wheelchair","motorized_wheelchair","auto_rickshaw","tuk","push","kick_scooter","razor","skateboard","board","skate","roller skate","footwear","derby","inline","bus_stop","wait","busstop","motorway","road","cupertino","interstate","highway","railway_track","oil_drum","barrell","fuel_pump","gas station","petroleum","fuelpump","petrol","wheel","transport","tire","turn","police_car_light","alert","error","pinged","beacon","car’s","emergency light","flashing","revolving","rotating","siren","warning","horizontal_traffic_light","signal","vertical_traffic_light","driving","semaphore","stop_sign","octagonal","caution","barrier","roadwork","striped","work_in_progress","anchor","ferry","boat","admiralty","fisherman","pattern","ring buoy","life saver","life preserver","float","rescue","sailboat","sailing","dinghy","resort","yacht","canoe","paddle","speedboat","motorboat","powerboat","passenger_ship","cruise","motor_boat","titanic","deploy","aeroplane","small_airplane","airplane_departure","airport","landing","departures","off","taking","airplane_arrival","boarding","arrivals","arriving","parachute","glide","hang","parasail","skydive","seat","sit","chair","helicopter","suspension_railway","mountain_cableway","cable","gondola","aerial_tramway","ropeway","satellite","gps","orbit","spaceflight","NASA","ISS","artificial","launch","staffmode","outer space","shuttle","flying_saucer","bellhop_bell","luggage","packing","suitcase","hourglass_done","time","oldschool","limit","timer","hourglass_not_done","countdown","flowing","accessories","apple","timepiece","wrist","wristwatch","alarm_clock","wake","stopwatch","deadline","timer_clock","mantelpiece_clock","twelve_o_clock","12","00:00","0000","12:00","1200","noon","midnight","midday","late","early","schedule","clock12","oclock","o’clock","twelve_thirty","00:30","0030","12:30","1230","clock1230","one_o_clock","1","1:00","13:00","1300","clock1","one_thirty","1:30","130","13:30","1330","clock130","two_o_clock","2","2:00","200","14:00","1400","clock2","two_thirty","2:30","230","14:30","1430","clock230","three_o_clock","3:00","300","15:00","1500","clock3","three_thirty","3:30","330","15:30","1530","clock330","four_o_clock","4","4:00","400","16:00","1600","clock4","four_thirty","4:30","430","16:30","1630","clock430","five_o_clock","5","5:00","500","17:00","1700","clock5","five_thirty","5:30","530","17:30","1730","clock530","six_o_clock","6","6:00","600","18:00","1800","clock6","six_thirty","6:30","630","18:30","1830","clock630","seven_o_clock","7","7:00","700","19:00","1900","clock7","seven_thirty","7:30","730","19:30","1930","clock730","eight_o_clock","8","8:00","800","20:00","2000","clock8","eight_thirty","8:30","830","20:30","2030","clock830","nine_o_clock","9","9:00","900","21:00","2100","clock9","nine_thirty","9:30","930","21:30","2130","clock930","ten_o_clock","10","10:00","1000","22:00","2200","clock10","ten_thirty","10:30","1030","22:30","2230","clock1030","eleven_o_clock","11","11:00","1100","23:00","2300","clock11","eleven_thirty","11:30","1130","23:30","2330","clock1130","new_moon","eclipse","shadow moon","solar","waxing_crescent_moon","first_quarter_moon","waxing_gibbous_moon","gray","full_moon","waning_gibbous_moon","last_quarter_moon","waning_crescent_moon","crescent_moon","new_moon_face","molester","first_quarter_moon_face","last_quarter_moon_face","brightness","rays","sunshine","full_moon_face","moonface","sun_with_face","sunface","ringed_planet","saturn","saturnine","medium","glowing_star","glittery","glow","shining","star2","shooting_star","meteoroid","upon","when","milky_way","galaxy","universe","cloudy","overcast","sun_behind_cloud","partly","cloud_with_lightning_and_rain","lightning","thunder","sun_behind_small_cloud","sun_behind_large_cloud","sun_behind_rain_cloud","cloud_with_rain","cloud_with_snow","cloud_with_lightning","tornado","cyclone","twister","whirlwind","wind_face","vortex","spiral","whirlpool","spin","hurricane","typhoon","rainbow","unicorn_face","primary","rain","closed_umbrella","drizzle","collapsed umbrella","umbrella","umbrella_with_rain_drops","rainy","raining","umbrella_on_ground","parasol","high_voltage","lightning bolt","zap","electricity","thunderbolt","snowflake","season","snowing","snowman","snowflakes","snowman_without_snow","without_snow","frosty","olaf","comet","flame","lit","snapstreak","remove","droplet","faucet","water_wave","tsunami","waves","jack_o_lantern","pumpkin","gourd","christmas_tree","december","xmas tree","fireworks","congratulations","sparkler","shine","firework","hanabi","senko","firecracker","dynamite","explosive","sparkles","shiny","glitter","party_popper","tada","confetti_ball","tanabata_tree","bamboo","star_festival","tanzaku","banner","pine_decoration","new_years","kadomatsu","year","japanese_dolls","kimono","doll","hinamatsuri","imperial","carp_streamer","koinobori","carp","flag","flags","socks","wind_chime","ding","bell","furin","moon_viewing_ceremony","tsukimi","dumplings","harvest","mid","scene","red_envelope","ang","hóngbāo","lai","packet","pao","ribbon","bowtie","wrapped_gift","present","reminder_ribbon","cause","awareness","admission_tickets","entrance","ticket","event","admission","stub","tour","military_medal","award","winning","army","medallion","trophy","win","contest","ftw","ceremony","championship","prize","winner","winners","sports_medal","1st_place_medal","first","2nd_place_medal","second","3rd_place_medal","third","bronze","soccer_ball","football","baseball","softball","glove","underarm","NBA","hoop","volleyball","american_football","NFL","gridiron","superbowl","rugby_football","league","union","tennis","racket","racquet","flying_disc","frisbee","ultimate","bowling","pin","pins","skittles","ten","cricket_game","field","field_hockey","ice_hockey","lacrosse","goal","ping_pong","pingpong","table","badminton","birdie","shuttlecock","boxing_glove","fighting","martial_arts_uniform","judo","karate","taekwondo","goal_net","flag_in_hole","ice_skate","skating","fishing_pole","rod","diving_mask","scuba","snorkeling","running_shirt","pageant","athletics","sash","singlet","skis","boot","skiing","sled","sleigh","luge","toboggan","sledge","curling_stone","direct_hit","target","bullseye","archery","dart","darts","yo_yo","fluctuate","yoyo","kite","soar","pistol","revolver","gun","handgun","squirt","pool_8_ball","8ball","billiard","billiards","cue","eight","snooker","crystal_ball","fortune_teller","clairvoyant","psychic","magic wand","supernature","video_game","console","PS4","controller","gamepad","playstation","u","wii","xbox","joystick","slot_machine","bet","vegas","fruit machine","casino","poker","game_die","dice","random","tabletop","puzzle_piece","interlocking","puzzle","clue","jigsaw","teddy_bear","plush","plaything","pinata","mexico","piñata","mirror ball","nesting dolls","matryoshka","russia","russian","spade_suit","cards","suits","card","spades","heart_suit","diamond_suit","diamonds","club_suit","clubs","chess_pawn","expendable","dupe","joker","playing","wildcard","mahjong_red_dragon","kanji","tile","flower_playing_cards","deck","hanafuda","hwatu","of cards","performing_arts","acting","theater","comedy","greek","logo","masks","theatre","theatre masks","tragedy","framed_picture","photography","frame","museum","artist_palette","colors","improve","thread","needle","sewing","spool","string","crafts","sewing needle","stitches","embroidery","sutures","tailoring","yarn","crochet","knit","knot","rope","scout","tangled","tie","twine","eyesight","eyeglasses","sunglasses","goggles","protection","swimming","welding","lab_coat","experiment","safety_vest","necktie","shirt","suitup","cloth","t_shirt","casual","tee","polo","tshirt","jeans","denim","pants","trousers","scarf","neck","clothes","gloves","coat","jacket","stockings","stocking","gown","skirt","dressing","sari","saree","shari","one_piece_swimsuit","briefs","brief","swimsuit","underwear","shorts","bikini","bathers","woman_s_clothes","shopping_bags","blouse","womans","woman’s","folding hand fan","flamenco","sensu","coin","wallet","handbag","accessory","bag","women’s","clutch_bag","pouch","buy","purchase","backpack","rucksack","satchel","thong sandal","flip","flops","jandals","sandals","thongs","zōri","man_s_shoe","mans","man’s","running_shoe","shoes","sneakers","athletic","sneaker","trainer","hiking_boot","backpacking","hiking","flat_shoe","ballet","slip-on","slipper","high_heeled_shoe","pumps","stiletto","heel","heels","woman_s_sandal","flip flops","heeled","shoe","ballet_shoes","pointe","woman_s_boot","boots","knee","hair pick","afro","comb","kod","leader","lord","woman_s_hat","ladies","top_hat","gentleman","tophat","wear","graduation_cap","college","degree","university","academic","square","billed_cap","military helmet","soldier","warrior","rescue_worker_s_helmet","aid","worker’s","prayer_beads","dhikr","religious","necklace","rosary","gloss","lip","makeup","propose","diamond","jewelry","gem","engagement","engaged","gem_stone","ruby","gemstone","jewel","muted_speaker","volume","cancellation","speaker_low_volume","broadcast","speaker_medium_volume","speaker","low","reduce","speaker_high_volume","noise","noisy","increase","loud","loudspeaker","address","announcement","bullhorn","megaphone","pa","system","cheering","mega","postal_horn","instrument","bugle","post","notification","chime","liberty","ringer","bell_with_slash","disabled","muted","notifications","musical_score","treble","clef","sheet","musical_note","tone","beamed","eighth","notes","quavers","musical_notes","singing","studio_microphone","recording","talkshow","mic","podcast","level_slider","scale","control_knobs","dial","microphone","PA","karaoke","headphone","gadgets","earbud","earphone","earphones","headphones","ipod","radio","program","digital","wireless","saxophone","blues","sax","accordion","accordian","concertina","squeeze","guitar","acoustic guitar","bass","musical_keyboard","piano","trumpet","brass","violin","orchestra","symphony","quartet","smallest","world’s","banjo","instructment","stringed","drum","drumsticks","snare","long drum","beat","conga","djembe","rhythm","maracas","percussion","flute","pied piper","recorder","harp","mobile_phone","iphone","smartphone","telephone","responsive_design","mobile_phone_with_arrow","incoming","call","calling","pointing","receive","rotary","telephone_receiver","handset","pager","bbcall","90s","beeper","bleeper","fax_machine","facsimile","battery","energy","sustain","aa","low battery","drained","electronic","electric_plug","charger","ac","adaptor","screen","display","monitor","desktop","notebook","pc","personal","desktop_computer","computing","imac","printer","ink","keyboard","type","input","computer_mouse","click","button","trackball","trackpad","computer_disk","record","data","disk","minidisc","minidisk","optical","floppy_disk","save","80s","optical_disk","dvd","disc","cd","compact","rom","abacus","calculation","count","counting","math","movie_camera","film","cinema","hollywood","film_frames","movie","strip","film_projector","tape","clapper_board","clapboard","director","slate","television","show","tv","camera_with_flash","snapshots","video_camera","camcorder","videocassette","vcr","vhs","magnifying_glass_tilted_left","search","zoom","find","icon","mag","magnifier","magnifying_glass_tilted_right","seo","candle","wax","light_bulb","idea","flashlight","sight","torch","red_paper_lantern","izakaya","diya_lamp","lighting","oil","notebook_with_decorative_cover","classroom","study","book","decorated","closed_book","read","library","knowledge","textbook","open_book","literature","novel","green_book","blue_book","orange_book","books","pile","stack","composition","ledger","binder","bound","page_with_curl","documents","office","curled","curly page","document","license","scroll","ancient","parchment","page_facing_up","printed","newspaper","press","headline","news","rolled_up_newspaper","delivery","bookmark_tabs","favorite","order","tidy","marker","bookmark","label","price","tag","sale","money_bag","payment","coins","moneybag","moneybags","currency","treasure","yen_banknote","banknotes","bill","dollar_banknote","euro_banknote","pound_banknote","sterling","bills","england","quid","twenty","money_with_wings","banknote","losing","credit_card","amex","diners","mastercard","subscription","visa","receipt","accounting","expenses","bookkeeping","evidence","proof","chart_increasing_with_yen","green-square","graph","presentation","stats","exchange","growth","market","rate","trend","upward","upwards","letter","inbox","✉ letter","e_mail","incoming_envelope","lines","envelope_with_arrow","downwards","insert","outgoing","sent","outbox_tray","inbox_tray","package","cardboard","moving","parcel","shipping","closed_mailbox_with_raised_flag","postbox","closed_mailbox_with_lowered_flag","open_mailbox_with_raised_flag","open_mailbox_with_lowered_flag","mailbox","ballot_box_with_ballot","election","vote","voting","pencil","writing","lead","pencil2","typos","black_nib","pen","✒ fountain","fountain_pen","lower","ballpoint","paintbrush","drawing","brush","crayon","memo","memorandum","documentation","briefcase","job","career","file_folder","directory","manilla","open_file_folder","load","card_index_dividers","organizing","calendar","day","july","tear_off_calendar","planning","spiral_notepad","pad","spiral_calendar","card_index","rolodex","chart_increasing","recovery","economics","success","metrics","positive chart","analytics","chart_decreasing","recession","failure","down pointing","negative chart","bar_chart","clipboard","pushpin","here","tack","round_pushpin","map","dropped","paperclip","clippy","linked_paperclips","link","straight_ruler","calculate","length","sketch","edge","triangular_ruler","set","triangle","scissors","card_file_box","database","file_cabinet","filing","wastebasket","bin","trash","rubbish","garbage","toss","basket","litter","wastepaper","locked","security","password","padlock","lock","privacy","unlocked","unlock","locked_with_pen","lock with","nib","locked_with_key","secure","key","door","old_key","hammer","tools","create","claw","handyman","axe","hatchet","pick","dig","mining","pickaxe","hammer_and_pick","hammer_and_wrench","spanner","dagger","crossed_swords","terrorism","boomerang","rebound","repercussion","bow_and_arrow","archer","sagittarius","shield","carpentry saw","carpenter","diy","ikea","fix","maintainer","screwdriver","screw","nut_and_bolt","handy","gear","cog","cogwheel","clamp","compress","compression","vice","winzip","balance_scale","fairness","libra","probing_cane","rings","url","chain","hyperlink","linked","broken chain","constraint","chains","hook","crook","curve","ensnare","selling","toolbox","chest","magnet","attraction","horseshoe","ladder","climb","rung","step","shovel","alembic","distilling","science","test_tube","petri_dish","biology","dna","genetics","life","double","evolution","gene","helix","microscope","laboratory","zoomin","investigate","magnify","telescope","astronomy","stargazing","satellite_antenna","future","dish","syringe","drugs","vaccination","vaccine","drop_of_blood","period","harm","wound","bleed","donation","menstruation","pill","pharmacy","drug","capsule","tablet","adhesive_bandage","heal","band","plaster","crutch","assist","cane","stethoscope","healthcheck","x-ray","bones","ray","entry","exit","doorway","elevator","hoist","mirror","reflection","reflector","speculum","window","scenery","opening","transparent","bedroom","couch_and_lamp","chill","lounge","settee","sofa","furniture","toilet","restroom","wc","washroom","potty","loo","plunger","force","suction","mouse trap","bait","mousetrap","sharp","shave","lotion_bottle","moisturizer","sunscreen","shampoo","safety_pin","diaper","punk","broom","cleaning","sweeping","sweep","laundry","roll_of_paper","towels","bucket","cask","vat","soap","lather","soapdish","bubbles","carbonation","burp","underwater","toothbrush","hygiene","dental","sponge","absorbing","porous","fire_extinguisher","quench","extinguish","shopping_cart","cigarette","kills","tobacco","joint","smoking","coffin","die","rip","graveyard","cemetery","casket","funeral","headstone","grave","tombstone","funeral_urn","ashes","nazar_amulet","bead","charm","boncuğu","talisman","hamsa","amulet","fatima","mary","miriam","moai","easter island","carving","moyai","statue","placard","demonstration","picket","protest","identification card","credentials","id","atm_sign","automated","teller","litter_in_bin_sign","its","litterbox","put","potable_water","drinking","tap","thirst","thirsty","wheelchair_symbol","access","accessible","men_s_room","lavatory","mens","men’s","women_s_room","womens","womens toilet","refresh","baby_symbol","orange-square","change","changing","nursery","water_closet","passport_control","custom","border","permissions","authorization","roles","customs","passport","baggage_claim","left_luggage","baggage","bag with","locker","exclamation","problem","issue","children_crossing","yellow-diamond","kids","traffic","experience","usability","no_entry","forbid","disallow","backslash","banned","block","restricted","no_bicycles","no_bikes","no_smoking","no_littering","do","non_potable_water","no_pedestrians","rules","crossing","no_mobile_phones","smartphones","no_one_under_eighteen","18","minor","age","nsfw","restriction","underage","radioactive","nuclear","radiation","biohazard","up_arrow","continue","cardinal","north","upgrade","up_right_arrow","diagonal","northeast","intercardinal","upper","right_arrow","next","right arrow","down_right_arrow","southeast","south","down_arrow","down arrow","downgrade","down_left_arrow","southwest","left arrow","west","left_arrow","previous","back","up_left_arrow","northwest","up_down_arrow","way","vertical","arrows","left_right_arrow","shape","horizontal","sideways","horizontal arrows","right_arrow_curving_left","return","undo","enter","curved","reply","left_arrow_curving_right","rotate","forward","right curved","right_arrow_curving_up","heading","then","right_arrow_curving_down","clockwise_vertical_arrows","sync","repeat","arrow","reload","counterclockwise_arrows_button","anticlockwise","switch","withershins","back_arrow","end_arrow","on_arrow","on!","soon_arrow","top_arrow","place_of_worship","atom_symbol","physics","atheist","om","sikhism","jainism","aumkara","hindu","omkara","pranava","star_of_david","magen","wheel_of_dharma","buddhist","helm","yin_yang","tao","taoist","latin_cross","christianity","orthodox_cross","suppedaneum","star_and_crescent","peace_symbol","hippie","menorah","hanukkah","branches","candelabrum","candlestick","chanukiah","nine","dotted_six_pointed_star","hexagram","fortune","khanda","Sikhism","astrology","gemini","twins","constellation","shuffle_tracks_button","shuffle","merge","repeat_button","loop","clockwise","retweet","repeat_single_button","circled","number","once","overlay","track","play_button","right triangle","fast_forward_button","next_track_button","skip","play_or_pause_button","pause","play/pause","reverse_button","backward","fast_reverse_button","rewind","revert","last_track_button","upwards_button","fast_up_button","downwards_button","fast_down_button","pause_button","stop_button","for","record_button","eject_button","curtain","stage","movies","dim_button","afternoon","decrease","bright_button","antenna_bars","reception","connection","bluetooth","bars","cellular","mobile","stairs","contactless","vibration_mode","mobile_phone_off","female_sign","venus","male_sign","mars","transgender symbol","transgender","lgbtq","multiplication_sign","cancel","multiply","plus_sign","addition","more","add","minus_sign","subtract","less","division_sign","divide","heavy equals sign","equality","infinity","forever","permanent","unbounded","universal","double_exclamation_mark","bangbang","exclamation_question_mark","wat","interrobang","question_mark","white_question_mark","doubts","white_exclamation_mark","exclamation_mark","heavy_exclamation_mark","wavy_dash","line","squiggle","scribble","currency_exchange","heavy_dollar_sign","medical_symbol","aesculapius","asclepius","asklepios","staff","recycling_symbol","recycle","reuse","fleur_de_lis","decorative","orleans","saints","scouts","trident_emblem","spear","pitchfork","name_badge","tofu","japanese_symbol_for_beginner","badge","chevron","shoshinsha","hollow_red_circle","correct","o","check_mark_button","answer","tick","pass_tests","check_box_with_check","confirm","black-square","ballot","checkbox","check_mark","nike","cross_mark","delete","multiplication","cross_mark_button","deny","negative","squared","curly_loop","curl","curling","double_curly_loop","cassette","voicemail","part_alternation_mark","m","mcdonald’s","eight_spoked_asterisk","eight_pointed_star","polygon","copyright","ip","c","registered","alphabet","r","trade_mark","trademark","tm","splatter","keycap_","twitter","hash","hashtag","octothorpe","pound","keycap","asterisk","keycap_0","0","null","zero","digit","keycap_1","keycap_2","prime","keycap_3","keycap_4","four","keycap_5","keycap_6","six","keycap_7","seven","keycap_8","keycap_9","keycap_10","input_latin_uppercase","letters","uppercase","abcd","capital","input_latin_lowercase","lowercase","input_numbers","1234","numeric","input_symbols","ampersand","percent","glyphs","characters","symbol input","input_latin_letters","abc","a_button","red-square","latin","ab_button","b_button","cl_button","clear","cool_button","free_button","source","tourist","id_button","identification","identity","circled_m","blue-circle","new_button","start","ng_button","blooper","o_button","o2","ok_button","p_button","parking","sos_button","distress","up_button","level","up!","vs_button","versus","japanese_here_button","katakana","destination","koko","meaning","word","“here”","japanese_service_charge_button","charge”","sa","“service","“service”","japanese_monthly_amount_button","month","amount”","cjk","ideograph","radical","u6708","unified","“monthly","japanese_not_free_of_charge_button","have","exist","own","u6709","“not","japanese_reserved_button","u6307","“reserved”","japanese_bargain_button","obtain","get","acquire","advantage","“bargain”","japanese_discount_button","pink-square","bargain","u5272","“discount”","japanese_free_of_charge_button","nothing","lacking","negation","u7121","“free","japanese_prohibited_button","prohibit","u7981","“prohibited”","japanese_acceptable_button","orange-circle","“acceptable”","japanese_application_button","apply","form","request","u7533","“application”","japanese_passing_grade_button","join","grade”","u5408","“passing","japanese_vacancy_button","7a7a","available","u7a7a","“vacancy”","japanese_congratulations_button","red-circle","congratulate","congratulation","“congratulations”","japanese_secret_button","sshh","“secret”","japanese_open_for_business_button","opening hours","55b6","business”","operating","u55b6","“open","japanese_no_vacancy_button","6e80","fullness","u6e80","vacancy”","“full;","“no","red_circle","geometric","orange_circle","yellow_circle","green_circle","blue_circle","purple_circle","brown_circle","black_circle","white_circle","red_square","orange_square","yellow_square","green_square","blue_square","purple_square","brown_square","black_large_square","white_large_square","black_medium_square","white_medium_square","black_medium_small_square","white_medium_small_square","black_small_square","white_small_square","large_orange_diamond","large_blue_diamond","small_orange_diamond","small_blue_diamond","red_triangle_pointed_up","red_triangle_pointed_down","diamond_with_a_dot","crystal","fancy","cuteness","inside","radio_button","white_square_button","black_square_button","chequered_flag","finishline","gokart","checkered","finish","grid","milestone","triangular_flag","pole","crossed_flags","black_flag","waving","white_flag","loser","lost","surrender","give up","rainbow_flag","queer","homosexual","bisexual","transgender flag","trans","pirate_flag","crossbones","jolly","plunder","roger","flag_ascension_island","flag_andorra","ad","andorra","andorran","flag_united_arab_emirates","united","emirates","united_arab_emirates","emirati","uae","flag_afghanistan","af","afghanistan","afghan","flag_antigua_barbuda","antigua","barbuda","antigua_barbuda","flag_anguilla","ai","anguilla","anguillan","flag_albania","al","albania","albanian","flag_armenia","armenia","armenian","flag_angola","ao","angola","angolan","flag_antarctica","aq","antarctica","antarctic","flag_argentina","ar","argentina","argentinian","flag_american_samoa","ws","american_samoa","samoan","flag_austria","at","austria","austrian","flag_australia","au","aussie","australian","heard","mcdonald","flag_aruba","aruba","aruban","flag_aland_islands","Åland","islands","aland_islands","flag_azerbaijan","az","azerbaijan","azerbaijani","flag_bosnia_herzegovina","bosnia","herzegovina","bosnia_herzegovina","flag_barbados","bb","barbados","bajan","barbadian","flag_bangladesh","bd","bangladesh","bangladeshi","flag_belgium","be","belgium","belgian","flag_burkina_faso","burkina","faso","burkina_faso","burkinabe","flag_bulgaria","bg","bulgaria","bulgarian","flag_bahrain","bh","bahrain","bahrainian","bahrani","flag_burundi","bi","burundi","burundian","flag_benin","bj","benin","beninese","flag_st_barthelemy","barthélemy","st_barthelemy","st.","flag_bermuda","bm","bermuda","bermudan flag","flag_brunei","bn","darussalam","brunei","bruneian","flag_bolivia","bo","bolivia","bolivian","flag_caribbean_netherlands","bonaire","caribbean_netherlands","eustatius","saba","sint","flag_brazil","br","brazil","brasil","brazilian","flag_bahamas","bs","bahamas","bahamian","flag_bhutan","bt","bhutan","bhutanese","flag_bouvet_island","norway","flag_botswana","bw","botswana","batswana","flag_belarus","by","belarus","belarusian","flag_belize","bz","belize","belizean","flag_canada","flag_cocos_islands","cocos","keeling","cocos_islands","island","flag_congo_kinshasa","congo","democratic","republic","congo_kinshasa","drc","flag_central_african_republic","central","central_african_republic","flag_congo_brazzaville","congo_brazzaville","flag_switzerland","ch","switzerland","flag_cote_d_ivoire","ivory","coast","cote_d_ivoire","côte","divoire","d’ivoire","flag_cook_islands","cook_islands","islander","flag_chile","chile","chilean","flag_cameroon","cm","cameroon","cameroonian","flag_china","china","prc","cn","indicator","regional","flag_colombia","co","colombia","colombian","flag_clipperton_island","flag_sark","cq","flag_costa_rica","costa","rica","costa_rica","rican","flag_cuba","cu","cuba","cuban","flag_cape_verde","cabo","verde","cape_verde","verdian","flag_curacao","curaçao","curacao","antilles","curaçaoan","flag_christmas_island","christmas_island","flag_cyprus","cy","cyprus","cypriot","flag_czechia","cz","czechia","czech","flag_germany","german","de","deutsch","flag_diego_garcia","flag_djibouti","dj","djibouti","djiboutian","flag_denmark","dk","denmark","danish","flag_dominica","dm","dominica","flag_dominican_republic","dominican","dominican_republic","dom","rep","flag_algeria","dz","algeria","algerian","flag_ceuta_melilla","flag_ecuador","ec","ecuador","ecuadorian","flag_estonia","ee","estonia","estonian","flag_egypt","eg","egypt","egyptian","flag_western_sahara","western","sahara","western_sahara","saharan","flag_eritrea","er","eritrea","eritrean","flag_spain","spain","ceuta","es","melilla","spanish","flag_ethiopia","ethiopia","ethiopian","flag_european_union","eu","flag_finland","fi","finland","finnish","flag_fiji","fj","fiji","fijian","flag_falkland_islands","falkland","malvinas","falkland_islands","falklander","falklands","islas","flag_micronesia","micronesia","federated","states","micronesian","flag_faroe_islands","faroe","faroe_islands","flag_france","clipperton","fr","martin","flag_gabon","ga","gabon","gabonese","flag_united_kingdom","kingdom","britain","northern","UK","english","union jack","united_kingdom","cornwall","scotland","wales","flag_grenada","gd","grenada","grenadian","flag_georgia","ge","georgia","georgian","flag_french_guiana","guiana","french_guiana","guinean","flag_guernsey","gg","guernsey","flag_ghana","gh","ghana","ghanaian","flag_gibraltar","gi","gibraltar","gibraltarian","flag_greenland","gl","greenland","greenlandic","flag_gambia","gm","gambia","gambian flag","flag_guinea","gn","guinea","flag_guadeloupe","gp","guadeloupe","guadeloupean","flag_equatorial_guinea","equatorial","equatorial_guinea","equatoguinean","flag_greece","gr","greece","flag_south_georgia_south_sandwich_islands","south_georgia_south_sandwich_islands","flag_guatemala","gt","guatemala","guatemalan","flag_guam","gu","guam","chamorro","guamanian","flag_guinea_bissau","gw","bissau","guinea_bissau","flag_guyana","gy","guyana","guyanese","flag_hong_kong_sar_china","hong","kong","hong_kong_sar_china","flag_heard_mcdonald_islands","flag_honduras","hn","honduras","honduran","flag_croatia","hr","croatia","croatian","flag_haiti","ht","haiti","haitian","flag_hungary","hu","hungary","hungarian","flag_canary_islands","canary","canary_islands","flag_indonesia","indonesia","indonesian","flag_ireland","ie","irish flag","flag_israel","il","israel","israeli","flag_isle_of_man","isle","isle_of_man","manx","flag_india","in","flag_british_indian_ocean_territory","territory","british_indian_ocean_territory","chagos","diego","garcia","flag_iraq","iq","iraq","iraqi","flag_iran","iran","islamic","iranian flag","flag_iceland","is","iceland","icelandic","flag_italy","flag_jersey","je","jersey","flag_jamaica","jm","jamaica","jamaican flag","flag_jordan","jo","jordan","jordanian","flag_japan","jp","ja","flag_kenya","ke","kenya","kenyan","flag_kyrgyzstan","kg","kyrgyzstan","kyrgyzstani","flag_cambodia","kh","cambodia","cambodian","flag_kiribati","ki","kiribati","flag_comoros","km","comoros","comoran","flag_st_kitts_nevis","kitts","nevis","st_kitts_nevis","flag_north_korea","korea","north_korea","korean","flag_south_korea","south_korea","kr","flag_kuwait","kw","kuwait","kuwaiti","flag_cayman_islands","cayman","cayman_islands","caymanian","flag_kazakhstan","kz","kazakhstan","kazakh","kazakhstani","flag_laos","lao","laos","laotian","flag_lebanon","lb","lebanon","lebanese","flag_st_lucia","lucia","st_lucia","flag_liechtenstein","li","liechtenstein","liechtensteiner","flag_sri_lanka","sri","lanka","sri_lanka","lankan","flag_liberia","lr","liberia","liberian","flag_lesotho","ls","lesotho","basotho","flag_lithuania","lt","lithuania","lithuanian","flag_luxembourg","lu","luxembourg","luxembourger","flag_latvia","lv","latvia","latvian","flag_libya","ly","libya","libyan","flag_morocco","morocco","moroccan","flag_monaco","mc","monaco","monégasque","flag_moldova","moldova","moldovan","flag_montenegro","me","montenegro","montenegrin","flag_st_martin","flag_madagascar","mg","madagascar","madagascan","flag_marshall_islands","marshall","marshall_islands","marshallese","flag_north_macedonia","macedonia","north_macedonia","macedonian","flag_mali","ml","mali","malian","flag_myanmar","mm","myanmar","burma","burmese","myanmarese flag","flag_mongolia","mn","mongolia","mongolian","flag_macao_sar_china","macao","macao_sar_china","macanese flag","macau","flag_northern_mariana_islands","mariana","northern_mariana_islands","flag_martinique","mq","martinique","martiniquais flag","of martinique","flag_mauritania","mr","mauritania","mauritanian","flag_montserrat","ms","montserrat","montserratian","flag_malta","mt","malta","maltese","flag_mauritius","mu","mauritian","flag_maldives","mv","maldives","maldivian","flag_malawi","mw","malawi","malawian flag","flag_mexico","mx","flag_malaysia","my","malaysia","malaysian","flag_mozambique","mz","mozambique","mozambican","flag_namibia","na","namibia","namibian","flag_new_caledonia","caledonia","new_caledonia","caledonian","flag_niger","ne","niger","nigerien flag","flag_norfolk_island","norfolk","norfolk_island","flag_nigeria","nigeria","nigerian","flag_nicaragua","ni","nicaragua","nicaraguan","flag_netherlands","nl","netherlands","dutch","flag_norway","bouvet","jan","mayen","norwegian","svalbard","flag_nepal","np","nepal","nepalese","flag_nauru","nr","nauru","nauruan","flag_niue","nu","niue","niuean","flag_new_zealand","zealand","new_zealand","kiwi","flag_oman","om_symbol","oman","omani","flag_panama","panama","panamanian","flag_peru","pe","peru","peruvian","flag_french_polynesia","polynesia","french_polynesia","polynesian","flag_papua_new_guinea","papua","papua_new_guinea","png","flag_philippines","ph","philippines","flag_pakistan","pk","pakistan","pakistani","flag_poland","pl","poland","polish","flag_st_pierre_miquelon","pierre","miquelon","st_pierre_miquelon","flag_pitcairn_islands","pitcairn","pitcairn_islands","flag_puerto_rico","puerto","rico","puerto_rico","flag_palestinian_territories","palestine","palestinian","territories","palestinian_territories","flag_portugal","pt","portugal","portugese","flag_palau","pw","palau","palauan","flag_paraguay","py","paraguay","paraguayan","flag_qatar","qa","qatar","qatari","flag_reunion","réunion","reunion","réunionnais","flag_romania","ro","romania","romanian","flag_serbia","rs","serbia","serbian flag","flag_russia","federation","ru","flag_rwanda","rw","rwanda","rwandan","flag_saudi_arabia","saudi_arabia","arabian flag","flag_solomon_islands","solomon","solomon_islands","islander flag","flag_seychelles","sc","seychelles","seychellois flag","flag_sudan","sd","sudan","sudanese","flag_sweden","se","swedish","flag_singapore","sg","singapore","singaporean","flag_st_helena","helena","ascension","tristan","cunha","st_helena","flag_slovenia","si","slovenia","slovenian","flag_svalbard_jan_mayen","flag_slovakia","sk","slovakia","slovakian","slovak flag","flag_sierra_leone","sierra","leone","sierra_leone","leonean","flag_san_marino","marino","san_marino","sammarinese","flag_senegal","sn","senegal","sengalese","flag_somalia","so","somalia","somalian flag","flag_suriname","sr","suriname","surinamer","flag_south_sudan","south_sudan","sudanese flag","flag_sao_tome_principe","sao","tome","principe","sao_tome_principe","príncipe","são","tomé","flag_el_salvador","el","salvador","el_salvador","salvadoran","flag_sint_maarten","maarten","sint_maarten","flag_syria","syrian","syria","flag_eswatini","sz","eswatini","swaziland","flag_tristan_da_cunha","flag_turks_caicos_islands","turks","caicos","turks_caicos_islands","flag_chad","td","chad","chadian","flag_french_southern_territories","southern","french_southern_territories","lands","flag_togo","tg","togo","togolese","flag_thailand","thailand","thai","flag_tajikistan","tj","tajikistan","tajik","flag_tokelau","tk","tokelau","tokelauan","flag_timor_leste","timor","leste","timor_leste","leste flag","timorese","flag_turkmenistan","turkmenistan","turkmen","flag_tunisia","tn","tunisia","tunisian","flag_tonga","to","tonga","tongan flag","flag_turkey","tr","turkish flag","türkiye","flag_trinidad_tobago","trinidad","tobago","trinidad_tobago","flag_tuvalu","tuvalu","tuvaluan","flag_taiwan","tw","taiwanese","flag_tanzania","tanzania","tanzanian","flag_ukraine","ua","ukraine","ukrainian","flag_uganda","ug","uganda","ugandan flag","flag_u_s_outlying_islands","u.s.","us","flag_united_nations","un","flag_united_states","united_states","outlying","usa","flag_uruguay","uy","uruguay","uruguayan","flag_uzbekistan","uz","uzbekistan","uzbek","uzbekistani","flag_vatican_city","vatican","vatican_city","vanticanien","flag_st_vincent_grenadines","vincent","grenadines","st_vincent_grenadines","flag_venezuela","ve","bolivarian","venezuela","venezuelan","flag_british_virgin_islands","bvi","british_virgin_islands","flag_u_s_virgin_islands","u_s_virgin_islands","flag_vietnam","viet","nam","vietnamese","flag_vanuatu","vu","vanuatu","vanuatu flag","flag_wallis_futuna","wallis","futuna","wallis_futuna","flag_samoa","samoa","samoan flag","flag_kosovo","xk","kosovo","kosovar","flag_yemen","ye","yemen","yemeni flag","flag_mayotte","yt","mayotte","flag_south_africa","africa","south_africa","african flag","flag_zambia","zm","zambia","zambian","flag_zimbabwe","zw","zimbabwe","zim","zimbabwean flag","flag_england","george's","st","flag_scotland","scottish","andrew's","saltire","flag_wales","welsh","baner","cymru","ddraig","goch","y"
};

static constexpr std::string_view EKW_0[] = {KW(0),KW(1),KW(2),KW(3),KW(4),KW(5),KW(6),KW(7)};
static constexpr std::string_view EKW_1[] = {KW(8),KW(1),KW(3),KW(4),KW(9),KW(5),KW(10),KW(2),KW(11),KW(12),KW(13),KW(7),KW(14)};
static constexpr std::string_view EKW_2[] = {KW(15),KW(1),KW(3),KW(4),KW(11),KW(9),KW(16),KW(17),KW(5),KW(10),KW(2),KW(18),KW(6),KW(12),KW(13),KW(19),KW(7)};
static constexpr std::string_view EKW_3[] = {KW(20),KW(1),KW(3),KW(2),KW(4),KW(21),KW(18),KW(6),KW(22)};
static constexpr std::string_view EKW_4[] = {KW(23),KW(3),KW(4),KW(24),KW(25),KW(9),KW(1),KW(26),KW(27),KW(16),KW(28),KW(29),KW(30),KW(6),KW(31),KW(12),KW(13),KW(2),KW(14),KW(32)};
static constexpr std::string_view EKW_5[] = {KW(33),KW(1),KW(34),KW(3),KW(16),KW(35),KW(2),KW(36),KW(37),KW(38),KW(12),KW(13),KW(14)};
static constexpr std::string_view EKW_6[] = {KW(39),KW(1),KW(40),KW(41),KW(31),KW(24),KW(9),KW(42),KW(16),KW(43)};
static constexpr std::string_view EKW_7[] = {KW(44),KW(1),KW(45),KW(46),KW(47),KW(3),KW(48),KW(9),KW(49),KW(16),KW(31),KW(24),KW(50)};
static constexpr std::string_view EKW_8[] = {KW(51),KW(1),KW(2),KW(52),KW(3),KW(53)};
static constexpr std::string_view EKW_9[] = {KW(54),KW(1),KW(55),KW(56),KW(2),KW(57)};
static constexpr std::string_view EKW_10[] = {KW(58),KW(34),KW(59),KW(60),KW(61),KW(62),KW(63),KW(64),KW(57)};
static constexpr std::string_view EKW_11[] = {KW(65),KW(1),KW(3),KW(66),KW(67),KW(68),KW(2),KW(18),KW(69),KW(70),KW(71)};
static constexpr std::string_view EKW_12[] = {KW(72),KW(1),KW(2),KW(3),KW(73),KW(74),KW(75),KW(76),KW(4),KW(77),KW(78),KW(18),KW(79),KW(7)};
static constexpr std::string_view EKW_13[] = {KW(80),KW(1),KW(81),KW(82),KW(83),KW(84),KW(85),KW(86),KW(2),KW(87)};
static constexpr std::string_view EKW_14[] = {KW(88),KW(1),KW(89),KW(17),KW(90),KW(91),KW(92),KW(74),KW(93),KW(94),KW(30),KW(95)};
static constexpr std::string_view EKW_15[] = {KW(96),KW(1),KW(89),KW(17),KW(90),KW(91),KW(92),KW(74),KW(97),KW(18),KW(98),KW(2)};
static constexpr std::string_view EKW_16[] = {KW(99),KW(1),KW(2),KW(100),KW(30),KW(22),KW(101),KW(102),KW(103)};
static constexpr std::string_view EKW_17[] = {KW(104),KW(1),KW(89),KW(17),KW(90),KW(91),KW(92),KW(105),KW(106),KW(69),KW(97),KW(107),KW(108)};
static constexpr std::string_view EKW_18[] = {KW(109),KW(89),KW(17),KW(1),KW(110),KW(91),KW(92),KW(105),KW(111),KW(112),KW(113)};
static constexpr std::string_view EKW_19[] = {KW(114),KW(1),KW(78),KW(115),KW(116),KW(3),KW(117),KW(19),KW(118),KW(2),KW(7),KW(119)};
static constexpr std::string_view EKW_20[] = {KW(120),KW(1),KW(89),KW(17),KW(90),KW(91),KW(92),KW(105),KW(18),KW(112)};
static constexpr std::string_view EKW_21[] = {KW(121),KW(1),KW(90),KW(91),KW(92),KW(105),KW(18),KW(112),KW(2),KW(122),KW(113)};
static constexpr std::string_view EKW_22[] = {KW(123),KW(124),KW(45),KW(125),KW(126),KW(3),KW(79),KW(127),KW(2),KW(128)};
static constexpr std::string_view EKW_23[] = {KW(129),KW(3),KW(4),KW(130),KW(2),KW(1),KW(56),KW(131),KW(132),KW(133),KW(134),KW(135),KW(136),KW(137),KW(138),KW(139),KW(14),KW(140),KW(141)};
static constexpr std::string_view EKW_24[] = {KW(142),KW(1),KW(143),KW(144),KW(145),KW(66),KW(2),KW(130),KW(146),KW(147),KW(148)};
static constexpr std::string_view EKW_25[] = {KW(149),KW(1),KW(143),KW(144),KW(145),KW(66),KW(2),KW(70),KW(130),KW(150),KW(18),KW(151),KW(147),KW(56),KW(148)};
static constexpr std::string_view EKW_26[] = {KW(152),KW(1),KW(135),KW(150),KW(101),KW(18),KW(30),KW(22),KW(153),KW(154),KW(155),KW(156),KW(157)};
static constexpr std::string_view EKW_27[] = {KW(158),KW(1),KW(143),KW(145),KW(66),KW(2),KW(130),KW(29),KW(18),KW(30),KW(159),KW(147),KW(148),KW(160),KW(32)};
static constexpr std::string_view EKW_28[] = {KW(161),KW(1),KW(162),KW(163),KW(164),KW(30),KW(165)};
static constexpr std::string_view EKW_29[] = {KW(166),KW(1),KW(2),KW(167),KW(168),KW(169),KW(13),KW(14)};
static constexpr std::string_view EKW_30[] = {KW(170),KW(1),KW(171),KW(172),KW(173),KW(174),KW(175),KW(30),KW(176),KW(14)};
static constexpr std::string_view EKW_31[] = {KW(177),KW(178),KW(67),KW(172),KW(173),KW(179),KW(180),KW(181),KW(182),KW(183),KW(184)};
static constexpr std::string_view EKW_32[] = {KW(185),KW(184),KW(186),KW(187),KW(76),KW(188),KW(189),KW(190)};
static constexpr std::string_view EKW_33[] = {KW(191),KW(1),KW(176),KW(192),KW(29),KW(175),KW(193),KW(194),KW(139),KW(195),KW(196),KW(178)};
static constexpr std::string_view EKW_34[] = {KW(197),KW(1),KW(198),KW(199),KW(200),KW(201),KW(202),KW(203),KW(108),KW(204)};
static constexpr std::string_view EKW_35[] = {KW(205),KW(206),KW(207),KW(208),KW(209),KW(210),KW(211)};
static constexpr std::string_view EKW_36[] = {KW(212),KW(1),KW(213),KW(214),KW(67),KW(194),KW(139),KW(178),KW(215)};
static constexpr std::string_view EKW_37[] = {KW(216),KW(1),KW(217),KW(218),KW(219),KW(181),KW(173),KW(220),KW(221),KW(222),KW(154),KW(223),KW(224)};
static constexpr std::string_view EKW_38[] = {KW(225),KW(226),KW(227),KW(228),KW(229),KW(230),KW(231),KW(12),KW(232)};
static constexpr std::string_view EKW_39[] = {KW(233),KW(1),KW(234),KW(235),KW(227),KW(230),KW(236),KW(12),KW(232),KW(237)};
static constexpr std::string_view EKW_40[] = {KW(238),KW(1),KW(239),KW(240),KW(241),KW(242),KW(176),KW(178),KW(243)};
static constexpr std::string_view EKW_41[] = {KW(244),KW(245),KW(246),KW(247),KW(248),KW(249),KW(60),KW(250),KW(251)};
static constexpr std::string_view EKW_42[] = {KW(252),KW(253),KW(254),KW(255),KW(256),KW(257),KW(258),KW(259),KW(260),KW(261),KW(262),KW(263)};
static constexpr std::string_view EKW_43[] = {KW(264),KW(1),KW(2),KW(265),KW(143),KW(266),KW(57),KW(267),KW(268),KW(269),KW(270)};
static constexpr std::string_view EKW_44[] = {KW(271),KW(226),KW(272),KW(273),KW(274),KW(57),KW(275),KW(276),KW(277),KW(278),KW(279),KW(280),KW(227),KW(281)};
static constexpr std::string_view EKW_45[] = {KW(282),KW(1),KW(283),KW(284),KW(18),KW(285)};
static constexpr std::string_view EKW_46[] = {KW(286),KW(1),KW(287),KW(288),KW(289),KW(290),KW(291)};
static constexpr std::string_view EKW_47[] = {KW(292),KW(293),KW(36),KW(294),KW(295),KW(296),KW(183),KW(297),KW(298),KW(122)};
static constexpr std::string_view EKW_48[] = {KW(299),KW(1),KW(300),KW(301),KW(302),KW(303),KW(304)};
static constexpr std::string_view EKW_49[] = {KW(305),KW(306),KW(172),KW(307),KW(308)};
static constexpr std::string_view EKW_50[] = {KW(309),KW(310),KW(311),KW(312)};
static constexpr std::string_view EKW_51[] = {KW(313),KW(314),KW(315)};
static constexpr std::string_view EKW_52[] = {KW(316),KW(1),KW(118),KW(317),KW(115),KW(116),KW(318),KW(19),KW(319)};
static constexpr std::string_view EKW_53[] = {KW(320),KW(1),KW(124),KW(249),KW(321),KW(322),KW(323),KW(324)};
static constexpr std::string_view EKW_54[] = {KW(325),KW(1),KW(294),KW(326),KW(327),KW(328),KW(329),KW(330),KW(331),KW(50)};
static constexpr std::string_view EKW_55[] = {KW(332),KW(1),KW(333)};
static constexpr std::string_view EKW_56[] = {KW(334),KW(1),KW(294),KW(335),KW(336),KW(337),KW(330),KW(338)};
static constexpr std::string_view EKW_57[] = {KW(339),KW(294),KW(335),KW(340)};
static constexpr std::string_view EKW_58[] = {KW(341),KW(1),KW(342),KW(343),KW(344),KW(345),KW(37),KW(346),KW(347),KW(348),KW(349)};
static constexpr std::string_view EKW_59[] = {KW(350),KW(342),KW(351),KW(352),KW(37),KW(353),KW(345),KW(343)};
static constexpr std::string_view EKW_60[] = {KW(354),KW(355),KW(356),KW(357),KW(358),KW(359),KW(360)};
static constexpr std::string_view EKW_61[] = {KW(361),KW(1),KW(362),KW(363),KW(364),KW(342),KW(365),KW(343),KW(366),KW(367),KW(368),KW(369)};
static constexpr std::string_view EKW_62[] = {KW(370),KW(1),KW(342),KW(366),KW(343),KW(12),KW(13),KW(371),KW(372),KW(108),KW(373),KW(362)};
static constexpr std::string_view EKW_63[] = {KW(374),KW(1),KW(375),KW(376),KW(342),KW(377),KW(378)};
static constexpr std::string_view EKW_64[] = {KW(379),KW(1),KW(380),KW(59),KW(381),KW(382),KW(383),KW(384)};
static constexpr std::string_view EKW_65[] = {KW(385),KW(1),KW(386),KW(387),KW(388),KW(389),KW(390),KW(391)};
static constexpr std::string_view EKW_66[] = {KW(392),KW(1),KW(306),KW(393),KW(394),KW(395),KW(396),KW(30),KW(397),KW(12),KW(398)};
static constexpr std::string_view EKW_67[] = {KW(399),KW(400),KW(401),KW(402),KW(306),KW(403),KW(404),KW(405),KW(30),KW(406),KW(147),KW(407)};
static constexpr std::string_view EKW_68[] = {KW(408),KW(342),KW(343),KW(409),KW(410),KW(411),KW(306),KW(412),KW(413),KW(414)};
static constexpr std::string_view EKW_69[] = {KW(415),KW(1),KW(416),KW(417),KW(418),KW(419),KW(420),KW(421)};
static constexpr std::string_view EKW_70[] = {KW(422),KW(1),KW(423),KW(424)};
static constexpr std::string_view EKW_71[] = {KW(425),KW(1),KW(426),KW(427),KW(428),KW(424),KW(429),KW(430)};
static constexpr std::string_view EKW_72[] = {KW(431),KW(432),KW(433),KW(434),KW(435),KW(436),KW(437),KW(304)};
static constexpr std::string_view EKW_73[] = {KW(438),KW(1),KW(439),KW(2),KW(440),KW(441),KW(442),KW(443),KW(444),KW(18),KW(445),KW(446),KW(434),KW(447),KW(448),KW(449),KW(450)};
static constexpr std::string_view EKW_74[] = {KW(451),KW(1),KW(452),KW(453),KW(454),KW(434),KW(14)};
static constexpr std::string_view EKW_75[] = {KW(455),KW(1),KW(456),KW(457),KW(162),KW(458),KW(459)};
static constexpr std::string_view EKW_76[] = {KW(460),KW(1),KW(226),KW(461),KW(462),KW(198),KW(463),KW(227),KW(464),KW(465),KW(466)};
static constexpr std::string_view EKW_77[] = {KW(467),KW(224),KW(468),KW(284),KW(234),KW(409),KW(469),KW(227),KW(276),KW(470)};
static constexpr std::string_view EKW_78[] = {KW(471),KW(1),KW(472),KW(291),KW(473),KW(124),KW(323)};
static constexpr std::string_view EKW_79[] = {KW(474),KW(1),KW(475),KW(469),KW(124),KW(321),KW(476),KW(281)};
static constexpr std::string_view EKW_80[] = {KW(477),KW(1),KW(124),KW(321),KW(476),KW(478),KW(281),KW(119)};
static constexpr std::string_view EKW_81[] = {KW(479),KW(1),KW(173),KW(480),KW(103),KW(414),KW(481),KW(482),KW(483)};
static constexpr std::string_view EKW_82[] = {KW(484),KW(1),KW(485),KW(195),KW(178),KW(486),KW(487),KW(173),KW(482)};
static constexpr std::string_view EKW_83[] = {KW(488),KW(1),KW(402),KW(482),KW(489),KW(490),KW(491),KW(183),KW(492),KW(416),KW(493)};
static constexpr std::string_view EKW_84[] = {KW(494),KW(1),KW(78),KW(76),KW(495),KW(174),KW(496),KW(75),KW(30),KW(13),KW(497),KW(498)};
static constexpr std::string_view EKW_85[] = {KW(499),KW(1),KW(500),KW(501),KW(45),KW(46),KW(124),KW(502),KW(30),KW(503),KW(504),KW(505)};
static constexpr std::string_view EKW_86[] = {KW(506),KW(128),KW(507),KW(45),KW(508),KW(79),KW(509),KW(124)};
static constexpr std::string_view EKW_87[] = {KW(510),KW(1),KW(511),KW(512),KW(476),KW(513)};
static constexpr std::string_view EKW_88[] = {KW(514),KW(1),KW(487),KW(291),KW(515)};
static constexpr std::string_view EKW_89[] = {KW(516),KW(1),KW(184),KW(517),KW(291),KW(518),KW(519),KW(416),KW(482)};
static constexpr std::string_view EKW_90[] = {KW(520),KW(1),KW(291),KW(35),KW(386),KW(37),KW(521),KW(12),KW(13),KW(522)};
static constexpr std::string_view EKW_91[] = {KW(523),KW(1),KW(317),KW(35),KW(291),KW(469),KW(524),KW(319)};
static constexpr std::string_view EKW_92[] = {KW(525),KW(1),KW(46),KW(124),KW(249),KW(321),KW(526),KW(45),KW(50)};
static constexpr std::string_view EKW_93[] = {KW(527),KW(528),KW(1),KW(45),KW(46),KW(124),KW(321),KW(249),KW(529),KW(530),KW(50)};
static constexpr std::string_view EKW_94[] = {KW(531),KW(1),KW(532),KW(184),KW(533),KW(534),KW(535),KW(536),KW(537),KW(538),KW(416)};
static constexpr std::string_view EKW_95[] = {KW(539),KW(1),KW(409),KW(342),KW(540),KW(519),KW(541),KW(12),KW(542),KW(543)};
static constexpr std::string_view EKW_96[] = {KW(544),KW(1),KW(342),KW(242),KW(321),KW(519),KW(30),KW(545),KW(546),KW(543),KW(547)};
static constexpr std::string_view EKW_97[] = {KW(548),KW(1),KW(124),KW(321),KW(249),KW(473),KW(323)};
static constexpr std::string_view EKW_98[] = {KW(549),KW(1),KW(34),KW(124),KW(294),KW(38),KW(37),KW(550),KW(551)};
static constexpr std::string_view EKW_99[] = {KW(552),KW(1),KW(294),KW(335),KW(124),KW(284),KW(321),KW(553),KW(554)};
static constexpr std::string_view EKW_100[] = {KW(555),KW(342),KW(556),KW(321),KW(284),KW(553),KW(340),KW(557),KW(373)};
static constexpr std::string_view EKW_101[] = {KW(558),KW(294),KW(335),KW(272),KW(559)};
static constexpr std::string_view EKW_102[] = {KW(560),KW(1),KW(561),KW(317),KW(79),KW(562),KW(563),KW(564),KW(284),KW(565),KW(566),KW(421),KW(266),KW(567),KW(568)};
static constexpr std::string_view EKW_103[] = {KW(569),KW(508),KW(421),KW(570),KW(571),KW(572),KW(573),KW(574),KW(575),KW(381)};
static constexpr std::string_view EKW_104[] = {KW(576),KW(421),KW(1),KW(577),KW(284),KW(578),KW(573)};
static constexpr std::string_view EKW_105[] = {KW(579),KW(1),KW(580),KW(581),KW(582),KW(583),KW(584),KW(175),KW(585),KW(586),KW(587),KW(274)};
static constexpr std::string_view EKW_106[] = {KW(588),KW(589),KW(590),KW(591),KW(85),KW(86),KW(3),KW(592),KW(593),KW(594),KW(2),KW(87)};
static constexpr std::string_view EKW_107[] = {KW(595),KW(589),KW(508),KW(590),KW(596),KW(591),KW(85),KW(86),KW(597),KW(592),KW(593),KW(124),KW(87)};
static constexpr std::string_view EKW_108[] = {KW(598),KW(405),KW(599),KW(600),KW(601),KW(405),KW(602),KW(603),KW(1),KW(85),KW(604),KW(605),KW(606),KW(607),KW(87)};
static constexpr std::string_view EKW_109[] = {KW(608),KW(607),KW(603),KW(609),KW(610),KW(601),KW(611),KW(591),KW(602),KW(1),KW(605),KW(606)};
static constexpr std::string_view EKW_110[] = {KW(612),KW(613),KW(614),KW(615),KW(616),KW(617),KW(618),KW(619),KW(620),KW(621),KW(622),KW(1),KW(606),KW(623),KW(14),KW(624),KW(625)};
static constexpr std::string_view EKW_111[] = {KW(626),KW(1),KW(627)};
static constexpr std::string_view EKW_112[] = {KW(628),KW(606),KW(381),KW(629),KW(605),KW(610),KW(600),KW(589),KW(596),KW(630),KW(631),KW(1),KW(85),KW(86),KW(632),KW(87),KW(633)};
static constexpr std::string_view EKW_113[] = {KW(597),KW(381),KW(591),KW(629),KW(606),KW(610),KW(600),KW(634),KW(631),KW(1),KW(85),KW(86),KW(303),KW(304),KW(87),KW(635)};
static constexpr std::string_view EKW_114[] = {KW(636),KW(605),KW(637),KW(610),KW(631),KW(60),KW(1),KW(85),KW(86),KW(638),KW(606),KW(87)};
static constexpr std::string_view EKW_115[] = {KW(639),KW(640),KW(641),KW(462),KW(642),KW(631),KW(643),KW(644),KW(1),KW(85),KW(86),KW(606),KW(87),KW(645)};
static constexpr std::string_view EKW_116[] = {KW(646),KW(647),KW(648),KW(649),KW(631),KW(644),KW(1),KW(85),KW(86),KW(650),KW(651),KW(652),KW(87),KW(653),KW(654)};
static constexpr std::string_view EKW_117[] = {KW(655),KW(656),KW(657),KW(658),KW(1),KW(606)};
static constexpr std::string_view EKW_118[] = {KW(659),KW(660),KW(661),KW(3),KW(2),KW(1),KW(12),KW(13),KW(7),KW(14)};
static constexpr std::string_view EKW_119[] = {KW(662),KW(660),KW(661),KW(2),KW(18),KW(1),KW(6),KW(3)};
static constexpr std::string_view EKW_120[] = {KW(663),KW(660),KW(661),KW(9),KW(3),KW(46),KW(1),KW(31),KW(50)};
static constexpr std::string_view EKW_121[] = {KW(664),KW(660),KW(89),KW(17),KW(90),KW(661),KW(91),KW(97),KW(18),KW(1),KW(665),KW(98),KW(2)};
static constexpr std::string_view EKW_122[] = {KW(666),KW(660),KW(661),KW(269),KW(1),KW(667),KW(668)};
static constexpr std::string_view EKW_123[] = {KW(669),KW(660),KW(661),KW(105),KW(29),KW(18),KW(30),KW(1)};
static constexpr std::string_view EKW_124[] = {KW(670),KW(660),KW(661),KW(532),KW(184),KW(538),KW(1),KW(518),KW(537),KW(671),KW(672),KW(482)};
static constexpr std::string_view EKW_125[] = {KW(673),KW(660),KW(46),KW(47),KW(124),KW(661),KW(321),KW(45),KW(1),KW(674),KW(50)};
static constexpr std::string_view EKW_126[] = {KW(675),KW(660),KW(661),KW(1),KW(573)};
static constexpr std::string_view EKW_127[] = {KW(676),KW(677),KW(660),KW(678),KW(9),KW(679),KW(175),KW(30),KW(1),KW(680),KW(681),KW(682),KW(683),KW(684),KW(685)};
static constexpr std::string_view EKW_128[] = {KW(686),KW(660),KW(677),KW(678),KW(175),KW(687),KW(688),KW(1),KW(680),KW(681),KW(689),KW(684),KW(685)};
static constexpr std::string_view EKW_129[] = {KW(690),KW(677),KW(660),KW(678),KW(533),KW(175),KW(1),KW(680),KW(681),KW(194),KW(691),KW(12),KW(241),KW(684),KW(692),KW(685),KW(682)};
static constexpr std::string_view EKW_130[] = {KW(693),KW(694),KW(17),KW(90),KW(695),KW(91),KW(97),KW(696),KW(697),KW(698)};
static constexpr std::string_view EKW_131[] = {KW(699),KW(89),KW(17),KW(97),KW(90),KW(91),KW(700),KW(701),KW(698)};
static constexpr std::string_view EKW_132[] = {KW(702),KW(89),KW(91),KW(703),KW(704),KW(705),KW(706),KW(707)};
static constexpr std::string_view EKW_133[] = {KW(708),KW(89),KW(17),KW(90),KW(91),KW(101),KW(709),KW(710),KW(711)};
static constexpr std::string_view EKW_134[] = {KW(712),KW(17),KW(89),KW(90),KW(91),KW(713),KW(101),KW(714),KW(715),KW(291),KW(716),KW(717)};
static constexpr std::string_view EKW_135[] = {KW(718),KW(89),KW(17),KW(90),KW(91),KW(713),KW(97),KW(719),KW(720),KW(721),KW(722)};
static constexpr std::string_view EKW_136[] = {KW(723),KW(89),KW(17),KW(90),KW(91),KW(97),KW(724)};
static constexpr std::string_view EKW_137[] = {KW(725),KW(89),KW(17),KW(90),KW(91),KW(97),KW(713),KW(155)};
static constexpr std::string_view EKW_138[] = {KW(726),KW(727),KW(89),KW(17)};
static constexpr std::string_view EKW_139[] = {KW(728),KW(729),KW(89),KW(730),KW(731),KW(732),KW(733),KW(734),KW(735),KW(736),KW(737),KW(381)};
static constexpr std::string_view EKW_140[] = {KW(738),KW(124),KW(739),KW(740),KW(97),KW(741),KW(742),KW(743)};
static constexpr std::string_view EKW_141[] = {KW(744),KW(745),KW(746),KW(747),KW(89),KW(748),KW(749)};
static constexpr std::string_view EKW_142[] = {KW(750),KW(751),KW(357),KW(752),KW(359),KW(753),KW(754),KW(755),KW(756),KW(757),KW(758),KW(759)};
static constexpr std::string_view EKW_143[] = {KW(760),KW(89),KW(17),KW(91),KW(761),KW(734)};
static constexpr std::string_view EKW_144[] = {KW(762),KW(91)};
static constexpr std::string_view EKW_145[] = {KW(763),KW(89),KW(17),KW(90),KW(91)};
static constexpr std::string_view EKW_146[] = {KW(764),KW(89),KW(17),KW(90),KW(91),KW(765),KW(766),KW(448)};
static constexpr std::string_view EKW_147[] = {KW(767),KW(89),KW(17),KW(90),KW(91),KW(768)};
static constexpr std::string_view EKW_148[] = {KW(769),KW(89),KW(17),KW(90),KW(91),KW(770),KW(229)};
static constexpr std::string_view EKW_149[] = {KW(771),KW(391),KW(772)};
static constexpr std::string_view EKW_150[] = {KW(773),KW(89),KW(17),KW(90),KW(91),KW(774),KW(775)};
static constexpr std::string_view EKW_151[] = {KW(776),KW(777)};
static constexpr std::string_view EKW_152[] = {KW(778),KW(591),KW(779),KW(780)};
static constexpr std::string_view EKW_153[] = {KW(781),KW(782),KW(783)};
static constexpr std::string_view EKW_154[] = {KW(784),KW(785)};
static constexpr std::string_view EKW_155[] = {KW(786),KW(1),KW(139),KW(89),KW(17),KW(90),KW(91),KW(97),KW(107),KW(787),KW(698)};
static constexpr std::string_view EKW_156[] = {KW(788),KW(789),KW(790),KW(791),KW(792),KW(793),KW(794),KW(795),KW(796),KW(797),KW(798),KW(799),KW(800),KW(801)};
static constexpr std::string_view EKW_157[] = {KW(802),KW(508),KW(421),KW(618),KW(803),KW(165),KW(804)};
static constexpr std::string_view EKW_158[] = {KW(805),KW(806),KW(807),KW(420),KW(418),KW(808),KW(809),KW(618),KW(810),KW(381),KW(811),KW(801),KW(740)};
static constexpr std::string_view EKW_159[] = {KW(306),KW(812),KW(709),KW(813),KW(814),KW(815),KW(618),KW(801),KW(816),KW(817)};
static constexpr std::string_view EKW_160[] = {KW(818),KW(819),KW(820),KW(519),KW(618),KW(821),KW(822),KW(823),KW(801),KW(824)};
static constexpr std::string_view EKW_161[] = {KW(825),KW(826),KW(827),KW(828),KW(829),KW(830),KW(831),KW(832),KW(106),KW(618),KW(833),KW(834),KW(835),KW(254),KW(801),KW(836)};
static constexpr std::string_view EKW_162[] = {KW(837),KW(187)};
static constexpr std::string_view EKW_163[] = {KW(838),KW(328),KW(839),KW(840),KW(841),KW(842),KW(843),KW(618),KW(844),KW(845),KW(846),KW(847)};
static constexpr std::string_view EKW_164[] = {KW(848),KW(849),KW(850),KW(851),KW(852)};
static constexpr std::string_view EKW_165[] = {KW(853),KW(839),KW(840),KW(841),KW(842),KW(845)};
static constexpr std::string_view EKW_166[] = {KW(854),KW(855),KW(856),KW(857),KW(421),KW(508),KW(858),KW(859),KW(860)};
static constexpr std::string_view EKW_167[] = {KW(861),KW(328),KW(862),KW(856),KW(857),KW(255),KW(618)};
static constexpr std::string_view EKW_168[] = {KW(337),KW(335),KW(294),KW(255),KW(863),KW(864),KW(618),KW(165),KW(330),KW(865),KW(801)};
static constexpr std::string_view EKW_169[] = {KW(866),KW(867),KW(168),KW(681),KW(868),KW(869),KW(870),KW(871),KW(872),KW(873),KW(602),KW(165)};
static constexpr std::string_view EKW_170[] = {KW(874),KW(875),KW(876),KW(877),KW(602)};
static constexpr std::string_view EKW_171[] = {KW(878),KW(879),KW(875),KW(873),KW(602),KW(193),KW(880),KW(876)};
static constexpr std::string_view EKW_172[] = {KW(881),KW(875),KW(882),KW(883),KW(873),KW(884),KW(602),KW(880),KW(885)};
static constexpr std::string_view EKW_173[] = {KW(886),KW(879),KW(875),KW(887),KW(888),KW(889),KW(602),KW(193),KW(890),KW(891),KW(892),KW(876),KW(893),KW(894)};
static constexpr std::string_view EKW_174[] = {KW(895),KW(873),KW(896),KW(897),KW(898)};
static constexpr std::string_view EKW_175[] = {KW(899),KW(873),KW(896),KW(312),KW(900)};
static constexpr std::string_view EKW_176[] = {KW(901),KW(873),KW(902),KW(903),KW(829)};
static constexpr std::string_view EKW_177[] = {KW(904),KW(905),KW(896),KW(906),KW(907),KW(908),KW(909)};
static constexpr std::string_view EKW_178[] = {KW(910),KW(883),KW(911),KW(882)};
static constexpr std::string_view EKW_179[] = {KW(912),KW(883),KW(911),KW(882)};
static constexpr std::string_view EKW_180[] = {KW(913),KW(875),KW(914),KW(790),KW(208),KW(915),KW(602),KW(165)};
static constexpr std::string_view EKW_181[] = {KW(916),KW(917),KW(918),KW(155),KW(919),KW(193),KW(681),KW(879),KW(920),KW(921),KW(922),KW(923),KW(924)};
static constexpr std::string_view EKW_182[] = {KW(925),KW(918),KW(155),KW(917),KW(926),KW(602),KW(927)};
static constexpr std::string_view EKW_183[] = {KW(928),KW(875),KW(929),KW(879),KW(930),KW(931),KW(724),KW(827),KW(602),KW(932),KW(165),KW(933)};
static constexpr std::string_view EKW_184[] = {KW(934),KW(935),KW(936),KW(602),KW(403),KW(193),KW(879),KW(937),KW(938),KW(939),KW(890)};
static constexpr std::string_view EKW_185[] = {KW(940),KW(97),KW(89),KW(164),KW(941),KW(942)};
static constexpr std::string_view EKW_186[] = {KW(943),KW(879),KW(875),KW(681),KW(602),KW(851),KW(944),KW(165)};
static constexpr std::string_view EKW_187[] = {KW(945),KW(879),KW(875),KW(946),KW(947),KW(948),KW(602),KW(589),KW(193),KW(734),KW(949)};
static constexpr std::string_view EKW_188[] = {KW(950),KW(168),KW(681),KW(951),KW(602),KW(952),KW(165)};
static constexpr std::string_view EKW_189[] = {KW(953),KW(954),KW(875),KW(879),KW(312),KW(602),KW(193),KW(955),KW(119)};
static constexpr std::string_view EKW_190[] = {KW(956),KW(875),KW(879),KW(954),KW(897),KW(602),KW(193),KW(955),KW(119)};
static constexpr std::string_view EKW_191[] = {KW(957),KW(875),KW(879),KW(954),KW(373),KW(602),KW(193),KW(890),KW(955),KW(119)};
static constexpr std::string_view EKW_192[] = {KW(958),KW(879),KW(875),KW(959),KW(890),KW(960),KW(961),KW(602),KW(962),KW(963),KW(964),KW(965),KW(966),KW(967)};
static constexpr std::string_view EKW_193[] = {KW(968),KW(875),KW(879),KW(954),KW(314),KW(602),KW(193),KW(955),KW(119)};
static constexpr std::string_view EKW_194[] = {KW(969),KW(879),KW(875),KW(954),KW(373),KW(602),KW(193),KW(955),KW(67),KW(119)};
static constexpr std::string_view EKW_195[] = {KW(970),KW(971),KW(972),KW(955)};
static constexpr std::string_view EKW_196[] = {KW(973),KW(974),KW(211),KW(975),KW(935),KW(976),KW(977),KW(439),KW(879),KW(17),KW(978),KW(979),KW(602),KW(208),KW(165),KW(204)};
static constexpr std::string_view EKW_197[] = {KW(980),KW(981),KW(242),KW(982),KW(879),KW(983),KW(624),KW(602),KW(984),KW(310),KW(165),KW(204)};
static constexpr std::string_view EKW_198[] = {KW(985),KW(875),KW(879),KW(986),KW(602),KW(987),KW(988),KW(989),KW(990)};
static constexpr std::string_view EKW_199[] = {KW(991),KW(508),KW(992),KW(993),KW(994),KW(995),KW(879),KW(602),KW(996),KW(997),KW(998),KW(987),KW(29),KW(999),KW(1000),KW(990),KW(165)};
static constexpr std::string_view EKW_200[] = {KW(1001),KW(879),KW(1002),KW(602),KW(998),KW(1003)};
static constexpr std::string_view EKW_201[] = {KW(1004),KW(879),KW(1002),KW(602),KW(998),KW(1005),KW(1006)};
static constexpr std::string_view EKW_202[] = {KW(1007),KW(168),KW(1008),KW(1009),KW(1010),KW(1011),KW(602),KW(1012),KW(1013),KW(879),KW(1014),KW(165)};
static constexpr std::string_view EKW_203[] = {KW(1015),KW(681),KW(1016),KW(1017),KW(426),KW(168),KW(827),KW(1018),KW(1019),KW(602),KW(1020),KW(1021),KW(1022),KW(879),KW(1023),KW(1024),KW(1008),KW(876),KW(724)};
static constexpr std::string_view EKW_204[] = {KW(1025),KW(89),KW(1026),KW(1027)};
static constexpr std::string_view EKW_205[] = {KW(1028),KW(875),KW(1029),KW(168),KW(13),KW(602),KW(879),KW(167),KW(1030),KW(165)};
static constexpr std::string_view EKW_206[] = {KW(1031),KW(168),KW(681),KW(1032),KW(1033),KW(602),KW(1034),KW(1035)};
static constexpr std::string_view EKW_207[] = {KW(1036),KW(1037),KW(1038),KW(1039),KW(879),KW(168),KW(1040),KW(1041)};
static constexpr std::string_view EKW_208[] = {KW(1042),KW(1043),KW(1044),KW(1045),KW(1046),KW(883),KW(1047),KW(1048),KW(1049),KW(1050),KW(1051),KW(602),KW(1052),KW(880),KW(681),KW(879),KW(885),KW(1024),KW(1033),KW(1053),KW(1054)};
static constexpr std::string_view EKW_209[] = {KW(1055),KW(1056),KW(1057),KW(1058),KW(1059),KW(602)};
static constexpr std::string_view EKW_210[] = {KW(1060),KW(1061),KW(1062),KW(1063),KW(193),KW(1064),KW(1065),KW(1066),KW(602),KW(1067),KW(875),KW(1068)};
static constexpr std::string_view EKW_211[] = {KW(1069),KW(1070),KW(952),KW(1071),KW(879)};
static constexpr std::string_view EKW_212[] = {KW(1072),KW(1071),KW(1073),KW(879),KW(440),KW(1074),KW(1075),KW(1076),KW(602),KW(618),KW(1077),KW(1078),KW(1079),KW(1080),KW(1081),KW(824)};
static constexpr std::string_view EKW_213[] = {KW(1082),KW(1083),KW(602),KW(1084)};
static constexpr std::string_view EKW_214[] = {KW(1085),KW(1083),KW(602),KW(1084)};
static constexpr std::string_view EKW_215[] = {KW(1086),KW(1087),KW(1088),KW(602)};
static constexpr std::string_view EKW_216[] = {KW(1089),KW(1087),KW(1090),KW(602)};
static constexpr std::string_view EKW_217[] = {KW(1091),KW(1),KW(1092),KW(1093),KW(1094),KW(602),KW(688),KW(1095),KW(1096),KW(304)};
static constexpr std::string_view EKW_218[] = {KW(1097),KW(1083),KW(602),KW(550)};
static constexpr std::string_view EKW_219[] = {KW(304),KW(1098),KW(1099),KW(602),KW(1100),KW(1101),KW(1102)};
static constexpr std::string_view EKW_220[] = {KW(257),KW(1103),KW(1104),KW(602),KW(1105)};
static constexpr std::string_view EKW_221[] = {KW(1106),KW(1107),KW(720),KW(1108),KW(1105),KW(716)};
static constexpr std::string_view EKW_222[] = {KW(1109),KW(1110),KW(1111),KW(1112),KW(1113),KW(1105),KW(1114)};
static constexpr std::string_view EKW_223[] = {KW(1115),KW(288),KW(1116),KW(602)};
static constexpr std::string_view EKW_224[] = {KW(1117),KW(599),KW(602)};
static constexpr std::string_view EKW_225[] = {KW(30),KW(566),KW(1118),KW(1119),KW(1120),KW(1121),KW(602),KW(18),KW(1122),KW(1),KW(1123),KW(498)};
static constexpr std::string_view EKW_226[] = {KW(18),KW(1),KW(566),KW(1121),KW(1118),KW(190),KW(602),KW(1124)};
static constexpr std::string_view EKW_227[] = {KW(130),KW(12),KW(145),KW(602),KW(147),KW(160)};
static constexpr std::string_view EKW_228[] = {KW(12),KW(105),KW(602),KW(107),KW(139)};
static constexpr std::string_view EKW_229[] = {KW(1125),KW(69),KW(1126),KW(1127),KW(1128),KW(1129),KW(518),KW(267),KW(291),KW(1130),KW(1131)};
static constexpr std::string_view EKW_230[] = {KW(1132),KW(1133),KW(1134),KW(1135),KW(1136),KW(1137),KW(1138)};
static constexpr std::string_view EKW_231[] = {KW(1133),KW(1139),KW(1138),KW(1134),KW(1140),KW(1135),KW(1141),KW(229),KW(1024),KW(1142)};
static constexpr std::string_view EKW_232[] = {KW(1134),KW(1143),KW(1144),KW(1145),KW(1146),KW(1133),KW(1138)};
static constexpr std::string_view EKW_233[] = {KW(1135),KW(1147),KW(1148),KW(1146),KW(1133),KW(1149),KW(1150),KW(1151),KW(1138),KW(1152)};
static constexpr std::string_view EKW_234[] = {KW(1024),KW(1139),KW(1153),KW(1147),KW(1140),KW(1141),KW(1144),KW(1143),KW(1154),KW(229),KW(1142),KW(1148),KW(1155)};
static constexpr std::string_view EKW_235[] = {KW(1156),KW(1157),KW(1158),KW(1159),KW(1143)};
static constexpr std::string_view EKW_236[] = {KW(1143),KW(1160),KW(1161),KW(1162),KW(1145),KW(1163),KW(1164),KW(435),KW(1153),KW(1144),KW(1154)};
static constexpr std::string_view EKW_237[] = {KW(1165),KW(1024),KW(1166),KW(1167)};
static constexpr std::string_view EKW_238[] = {KW(1168),KW(1169),KW(1167),KW(1166),KW(1144),KW(1154)};
static constexpr std::string_view EKW_239[] = {KW(1170),KW(1169),KW(1167),KW(1166),KW(1147),KW(1155)};
static constexpr std::string_view EKW_240[] = {KW(1171),KW(1157),KW(1153),KW(1172),KW(1159),KW(1144),KW(1154),KW(1173)};
static constexpr std::string_view EKW_241[] = {KW(1174),KW(1157),KW(1153),KW(1159),KW(1144),KW(1154)};
static constexpr std::string_view EKW_242[] = {KW(1175),KW(1176),KW(1177),KW(1153),KW(1159),KW(1144),KW(1154)};
static constexpr std::string_view EKW_243[] = {KW(1178),KW(1179),KW(1153),KW(1180),KW(1144),KW(1154),KW(242)};
static constexpr std::string_view EKW_244[] = {KW(1148),KW(1147),KW(1181),KW(1182),KW(1153),KW(1155),KW(1183)};
static constexpr std::string_view EKW_245[] = {KW(1184),KW(1157),KW(1153),KW(1147),KW(1172),KW(1159),KW(1173),KW(1155)};
static constexpr std::string_view EKW_246[] = {KW(1185),KW(1157),KW(1153),KW(1140),KW(1159),KW(1142)};
static constexpr std::string_view EKW_247[] = {KW(1186),KW(1157),KW(1153),KW(1147),KW(1159),KW(1155)};
static constexpr std::string_view EKW_248[] = {KW(1187),KW(1157),KW(1153),KW(1140),KW(1159),KW(1142)};
static constexpr std::string_view EKW_249[] = {KW(1188),KW(1176),KW(1177),KW(1153),KW(1147),KW(1159),KW(1155)};
static constexpr std::string_view EKW_250[] = {KW(1189),KW(1177),KW(1176),KW(1153),KW(1140),KW(1159),KW(1142)};
static constexpr std::string_view EKW_251[] = {KW(1190),KW(1179),KW(1153),KW(1147),KW(1180),KW(242),KW(1155)};
static constexpr std::string_view EKW_252[] = {KW(1191),KW(1179),KW(1153),KW(1140),KW(1180),KW(242),KW(1142)};
static constexpr std::string_view EKW_253[] = {KW(1192),KW(1148),KW(1147),KW(1135),KW(1158),KW(1024),KW(1159),KW(1155)};
static constexpr std::string_view EKW_254[] = {KW(1193),KW(1143),KW(1144),KW(1134),KW(1158),KW(1145),KW(1024),KW(1159),KW(1154)};
static constexpr std::string_view EKW_255[] = {KW(1194),KW(1195),KW(1177),KW(1196),KW(1139),KW(1153),KW(1147),KW(1140),KW(1144),KW(1143),KW(1154),KW(229),KW(1176),KW(1142),KW(1148),KW(1155)};
static constexpr std::string_view EKW_256[] = {KW(1197),KW(1195),KW(1144),KW(1154),KW(1176),KW(1177),KW(1196),KW(1153),KW(1198),KW(1199),KW(1200)};
static constexpr std::string_view EKW_257[] = {KW(1201),KW(1195),KW(1147),KW(1155),KW(1182),KW(1176),KW(1177),KW(1196),KW(1153),KW(1198),KW(1202),KW(1203),KW(1200)};
static constexpr std::string_view EKW_258[] = {KW(1204),KW(1131),KW(476),KW(681),KW(124),KW(1148)};
static constexpr std::string_view EKW_259[] = {KW(1205),KW(1144),KW(1134),KW(1143),KW(124),KW(249),KW(1206),KW(281),KW(476),KW(681),KW(1154)};
static constexpr std::string_view EKW_260[] = {KW(1207),KW(1147),KW(1135),KW(1148),KW(124),KW(249),KW(1206),KW(281),KW(476),KW(681),KW(1155)};
static constexpr std::string_view EKW_261[] = {KW(1208),KW(321),KW(239),KW(1),KW(557),KW(681),KW(566),KW(373)};
static constexpr std::string_view EKW_262[] = {KW(1209),KW(1144),KW(1134),KW(1143),KW(681),KW(1154)};
static constexpr std::string_view EKW_263[] = {KW(1210),KW(1147),KW(1135),KW(1148),KW(681),KW(1155)};
static constexpr std::string_view EKW_264[] = {KW(1211),KW(1212),KW(1018),KW(1039),KW(1213),KW(1),KW(680),KW(681),KW(935),KW(1214),KW(879),KW(684),KW(208),KW(685),KW(882),KW(1215)};
static constexpr std::string_view EKW_265[] = {KW(1216),KW(1144),KW(1134),KW(1143),KW(1217),KW(1213),KW(680),KW(681),KW(935),KW(1214),KW(879),KW(1154),KW(1218),KW(684),KW(208),KW(685),KW(882)};
static constexpr std::string_view EKW_266[] = {KW(1219),KW(1147),KW(1135),KW(1148),KW(1217),KW(1213),KW(680),KW(681),KW(935),KW(1214),KW(879),KW(1218),KW(684),KW(208),KW(685),KW(882),KW(1155)};
static constexpr std::string_view EKW_267[] = {KW(1220),KW(976),KW(1221),KW(1),KW(681),KW(879),KW(168),KW(261)};
static constexpr std::string_view EKW_268[] = {KW(1222),KW(1154),KW(1134),KW(1144),KW(386),KW(1195),KW(1143),KW(681),KW(879)};
static constexpr std::string_view EKW_269[] = {KW(1223),KW(1155),KW(1135),KW(1147),KW(713),KW(1195),KW(1148),KW(681),KW(879)};
static constexpr std::string_view EKW_270[] = {KW(1224),KW(1225),KW(1226),KW(1227),KW(1228),KW(1229),KW(1147),KW(1230),KW(1135),KW(1180),KW(1231),KW(1232),KW(1148),KW(1155)};
static constexpr std::string_view EKW_271[] = {KW(1233),KW(1144),KW(1134),KW(1143),KW(1195),KW(1225),KW(1229),KW(1231),KW(1154),KW(1232)};
static constexpr std::string_view EKW_272[] = {KW(1234),KW(1147),KW(1135),KW(1148),KW(1195),KW(1225),KW(1229),KW(1231),KW(1232),KW(1155)};
static constexpr std::string_view EKW_273[] = {KW(1235),KW(1236),KW(1237),KW(681),KW(3),KW(154),KW(876),KW(373)};
static constexpr std::string_view EKW_274[] = {KW(1238),KW(1144),KW(1134),KW(1143),KW(681),KW(3),KW(1154),KW(154),KW(876)};
static constexpr std::string_view EKW_275[] = {KW(1239),KW(1147),KW(1135),KW(1148),KW(681),KW(3),KW(154),KW(876),KW(1155)};
static constexpr std::string_view EKW_276[] = {KW(1240),KW(1083),KW(1091),KW(1092)};
static constexpr std::string_view EKW_277[] = {KW(1241),KW(1083),KW(1144),KW(1154)};
static constexpr std::string_view EKW_278[] = {KW(1242),KW(1083),KW(1147),KW(1155)};
static constexpr std::string_view EKW_279[] = {KW(1243),KW(1244),KW(1245),KW(1052),KW(1134),KW(1246),KW(1247),KW(1248),KW(681),KW(1143),KW(115),KW(206),KW(739),KW(1049)};
static constexpr std::string_view EKW_280[] = {KW(1249),KW(1143),KW(1144),KW(1134),KW(1245),KW(1052),KW(1247),KW(1250),KW(681),KW(1154),KW(206),KW(739),KW(1049)};
static constexpr std::string_view EKW_281[] = {KW(1251),KW(1148),KW(1147),KW(1135),KW(1245),KW(1052),KW(1247),KW(1250),KW(681),KW(206),KW(739),KW(1049),KW(1155)};
static constexpr std::string_view EKW_282[] = {KW(1252),KW(469),KW(181),KW(1253),KW(1),KW(1254),KW(261),KW(1255),KW(873),KW(1256),KW(1257)};
static constexpr std::string_view EKW_283[] = {KW(1258),KW(1143),KW(1144),KW(1134),KW(181),KW(1253),KW(1),KW(1254),KW(1154),KW(873)};
static constexpr std::string_view EKW_284[] = {KW(1259),KW(1148),KW(1147),KW(1135),KW(181),KW(1253),KW(1),KW(1254),KW(873),KW(1155)};
static constexpr std::string_view EKW_285[] = {KW(1260),KW(1261),KW(1262),KW(1263),KW(226),KW(1264),KW(1265)};
static constexpr std::string_view EKW_286[] = {KW(1266),KW(1143),KW(1144),KW(1134),KW(409),KW(234),KW(1262),KW(1263),KW(226),KW(1154),KW(1264)};
static constexpr std::string_view EKW_287[] = {KW(1267),KW(1148),KW(1147),KW(1135),KW(409),KW(234),KW(1262),KW(1263),KW(226),KW(1264),KW(1155)};
static constexpr std::string_view EKW_288[] = {KW(1268),KW(1269),KW(1116),KW(347),KW(1270),KW(1271),KW(1272),KW(1273),KW(1274),KW(1275)};
static constexpr std::string_view EKW_289[] = {KW(1276),KW(347),KW(1272),KW(1275),KW(1270),KW(1143),KW(1195),KW(1116),KW(1144),KW(1271),KW(1154),KW(1273),KW(1274)};
static constexpr std::string_view EKW_290[] = {KW(1277),KW(347),KW(1272),KW(1275),KW(1270),KW(1148),KW(1195),KW(1116),KW(1147),KW(1271),KW(1273),KW(1274),KW(1155)};
static constexpr std::string_view EKW_291[] = {KW(1278),KW(1279),KW(1280),KW(1281),KW(1282),KW(1283)};
static constexpr std::string_view EKW_292[] = {KW(1284),KW(1281),KW(1143),KW(1195),KW(1280),KW(1285),KW(1144),KW(1154),KW(1282),KW(1283)};
static constexpr std::string_view EKW_293[] = {KW(1286),KW(1281),KW(1148),KW(1195),KW(1280),KW(1147),KW(1285),KW(1282),KW(1283),KW(1155)};
static constexpr std::string_view EKW_294[] = {KW(1287),KW(1288),KW(1280),KW(1289),KW(1290)};
static constexpr std::string_view EKW_295[] = {KW(1291),KW(1290),KW(1288),KW(1143),KW(1195),KW(1280),KW(1289),KW(1144),KW(1154),KW(1283)};
static constexpr std::string_view EKW_296[] = {KW(1292),KW(1290),KW(1288),KW(1148),KW(1195),KW(1280),KW(1289),KW(1147),KW(1283),KW(1155)};
static constexpr std::string_view EKW_297[] = {KW(1293),KW(1294),KW(1295),KW(1296),KW(1297)};
static constexpr std::string_view EKW_298[] = {KW(1298),KW(1296),KW(1295),KW(1143),KW(1195),KW(1294),KW(1144),KW(1154),KW(1297)};
static constexpr std::string_view EKW_299[] = {KW(1299),KW(1296),KW(1295),KW(1148),KW(1195),KW(1147),KW(1294),KW(1297),KW(1155)};
static constexpr std::string_view EKW_300[] = {KW(1300),KW(1301),KW(1302),KW(1303),KW(1304),KW(1305),KW(1306)};
static constexpr std::string_view EKW_301[] = {KW(1307),KW(1305),KW(1304),KW(1143),KW(1195),KW(1302),KW(1303),KW(1144),KW(1154),KW(1306)};
static constexpr std::string_view EKW_302[] = {KW(1308),KW(1305),KW(1304),KW(1148),KW(1195),KW(1302),KW(1303),KW(1147),KW(1155),KW(1306)};
static constexpr std::string_view EKW_303[] = {KW(1309),KW(1310),KW(1311),KW(1312),KW(1313),KW(1314),KW(1315)};
static constexpr std::string_view EKW_304[] = {KW(1316),KW(1313),KW(1143),KW(1195),KW(1314),KW(1310),KW(1144),KW(1154),KW(1315)};
static constexpr std::string_view EKW_305[] = {KW(1317),KW(1313),KW(1148),KW(1195),KW(1314),KW(1147),KW(1310),KW(1315),KW(1155)};
static constexpr std::string_view EKW_306[] = {KW(1318),KW(1306),KW(1319),KW(1320),KW(1024),KW(1321),KW(1322),KW(1323)};
static constexpr std::string_view EKW_307[] = {KW(1324),KW(1321),KW(1143),KW(1195),KW(1325),KW(1320),KW(1144),KW(1154),KW(1024),KW(1322),KW(1323)};
static constexpr std::string_view EKW_308[] = {KW(1326),KW(1321),KW(1148),KW(1195),KW(1325),KW(1320),KW(1147),KW(1024),KW(1322),KW(1323),KW(1155)};
static constexpr std::string_view EKW_309[] = {KW(1327),KW(1328),KW(1329),KW(1330),KW(1331)};
static constexpr std::string_view EKW_310[] = {KW(1332),KW(1329),KW(1330),KW(1143),KW(1195),KW(1144),KW(1154),KW(1331)};
static constexpr std::string_view EKW_311[] = {KW(1333),KW(1329),KW(1330),KW(1148),KW(1195),KW(1147),KW(1331),KW(1155)};
static constexpr std::string_view EKW_312[] = {KW(1334),KW(1335),KW(1336),KW(1337),KW(1338),KW(1339),KW(1340),KW(1341),KW(1342)};
static constexpr std::string_view EKW_313[] = {KW(1343),KW(1335),KW(1342),KW(1143),KW(1195),KW(1336),KW(1337),KW(1338),KW(1339),KW(1340),KW(1344),KW(1345),KW(1341),KW(1144),KW(1154)};
static constexpr std::string_view EKW_314[] = {KW(1346),KW(1335),KW(1342),KW(1148),KW(1195),KW(1336),KW(1337),KW(1338),KW(1339),KW(1340),KW(1347),KW(1345),KW(1341),KW(1147),KW(1155)};
static constexpr std::string_view EKW_315[] = {KW(1348),KW(1349),KW(1350),KW(1351),KW(1352),KW(1353),KW(1354),KW(1355),KW(1319)};
static constexpr std::string_view EKW_316[] = {KW(1356),KW(1350),KW(1351),KW(1352),KW(1355),KW(1143),KW(1195),KW(1353),KW(1144),KW(1354),KW(1154),KW(1357),KW(1319)};
static constexpr std::string_view EKW_317[] = {KW(1358),KW(1350),KW(1351),KW(1352),KW(1355),KW(1148),KW(1195),KW(1147),KW(1353),KW(1354),KW(1357),KW(1319),KW(1155)};
static constexpr std::string_view EKW_318[] = {KW(1359),KW(656),KW(1360),KW(1352),KW(1361),KW(1362),KW(1363),KW(1364)};
static constexpr std::string_view EKW_319[] = {KW(1365),KW(1360),KW(1364),KW(1352),KW(1366),KW(1362),KW(1143),KW(1195),KW(1361),KW(656),KW(1367),KW(1144),KW(1154),KW(1363)};
static constexpr std::string_view EKW_320[] = {KW(1368),KW(1360),KW(1364),KW(1352),KW(1366),KW(1362),KW(1148),KW(1195),KW(1361),KW(656),KW(1367),KW(1147),KW(1363),KW(1155)};
static constexpr std::string_view EKW_321[] = {KW(1369),KW(1370),KW(1371),KW(1372),KW(1373),KW(1374),KW(1375),KW(1376),KW(223),KW(1377),KW(1378),KW(812)};
static constexpr std::string_view EKW_322[] = {KW(1379),KW(1378),KW(1374),KW(1143),KW(1195),KW(1373),KW(1380),KW(1381),KW(1144),KW(1154),KW(1375),KW(1376),KW(223),KW(1377),KW(1382),KW(812)};
static constexpr std::string_view EKW_323[] = {KW(1383),KW(1378),KW(1374),KW(1148),KW(1195),KW(1373),KW(1147),KW(1375),KW(1376),KW(223),KW(1377),KW(812),KW(1155)};
static constexpr std::string_view EKW_324[] = {KW(1371),KW(1384),KW(1385),KW(1386),KW(1387),KW(1388),KW(1389),KW(1390)};
static constexpr std::string_view EKW_325[] = {KW(1391),KW(1389),KW(1143),KW(1195),KW(1387),KW(1144),KW(1154),KW(1388),KW(1390)};
static constexpr std::string_view EKW_326[] = {KW(1392),KW(1389),KW(1148),KW(1195),KW(1387),KW(1147),KW(1388),KW(1390),KW(1155)};
static constexpr std::string_view EKW_327[] = {KW(1393),KW(1394),KW(1395),KW(1396),KW(1397),KW(1398)};
static constexpr std::string_view EKW_328[] = {KW(1399),KW(1398),KW(1395),KW(1143),KW(1195),KW(1396),KW(1397),KW(1144),KW(1154)};
static constexpr std::string_view EKW_329[] = {KW(1400),KW(1398),KW(1395),KW(1148),KW(1195),KW(1396),KW(1397),KW(1147),KW(1155)};
static constexpr std::string_view EKW_330[] = {KW(1401),KW(1402),KW(1403),KW(1404),KW(1405),KW(652),KW(1406)};
static constexpr std::string_view EKW_331[] = {KW(1407),KW(652),KW(1405),KW(1143),KW(1195),KW(1408),KW(1144),KW(1154),KW(1403),KW(1404),KW(1406)};
static constexpr std::string_view EKW_332[] = {KW(1409),KW(652),KW(1405),KW(1148),KW(1195),KW(1408),KW(1147),KW(1403),KW(1404),KW(1406),KW(1155)};
static constexpr std::string_view EKW_333[] = {KW(1410),KW(1411),KW(1412)};
static constexpr std::string_view EKW_334[] = {KW(1413),KW(1414),KW(1143),KW(1195),KW(1411),KW(1412),KW(1144),KW(1154)};
static constexpr std::string_view EKW_335[] = {KW(1415),KW(1414),KW(1148),KW(1195),KW(1147),KW(1411),KW(1412),KW(1155)};
static constexpr std::string_view EKW_336[] = {KW(1416),KW(1417),KW(1294),KW(1418),KW(1419)};
static constexpr std::string_view EKW_337[] = {KW(1420),KW(1143),KW(1421),KW(1294),KW(1422),KW(1423),KW(1424),KW(1425),KW(1417),KW(1144),KW(1154),KW(1418)};
static constexpr std::string_view EKW_338[] = {KW(1426),KW(1148),KW(1421),KW(1294),KW(1422),KW(1423),KW(1424),KW(1425),KW(1147),KW(1417),KW(1419),KW(1155)};
static constexpr std::string_view EKW_339[] = {KW(1427),KW(1195),KW(1428),KW(18),KW(1429),KW(1430),KW(1431)};
static constexpr std::string_view EKW_340[] = {KW(1432),KW(1433),KW(1144),KW(1154),KW(1431),KW(1428)};
static constexpr std::string_view EKW_341[] = {KW(1434),KW(1195),KW(1428),KW(1427),KW(1147),KW(1148),KW(1431),KW(1155)};
static constexpr std::string_view EKW_342[] = {KW(1435),KW(1436),KW(1437),KW(1438)};
static constexpr std::string_view EKW_343[] = {KW(1439),KW(1440),KW(1441),KW(1437),KW(1144),KW(1145),KW(1442),KW(1438),KW(1154)};
static constexpr std::string_view EKW_344[] = {KW(1443),KW(1440),KW(1441),KW(1437),KW(1147),KW(1442),KW(1148),KW(1438),KW(1444),KW(1155)};
static constexpr std::string_view EKW_345[] = {KW(1445),KW(1446),KW(1447),KW(1448),KW(1449),KW(1450),KW(1451)};
static constexpr std::string_view EKW_346[] = {KW(1452),KW(1328),KW(1453),KW(1454),KW(1),KW(550),KW(424),KW(1455),KW(1456),KW(1457),KW(1458)};
static constexpr std::string_view EKW_347[] = {KW(1459),KW(1144),KW(1195),KW(1460),KW(1145),KW(1453),KW(1461),KW(1306),KW(1328),KW(1455),KW(1154)};
static constexpr std::string_view EKW_348[] = {KW(1462),KW(1147),KW(1195),KW(1460),KW(1453),KW(1461),KW(1306),KW(1328),KW(1148),KW(1455),KW(1155)};
static constexpr std::string_view EKW_349[] = {KW(1463),KW(1464),KW(988),KW(1465),KW(1466),KW(1467)};
static constexpr std::string_view EKW_350[] = {KW(1468),KW(1134),KW(1143),KW(1144),KW(1469),KW(1442),KW(1470),KW(85),KW(86),KW(1154),KW(87)};
static constexpr std::string_view EKW_351[] = {KW(1471),KW(1135),KW(1148),KW(1147),KW(1472),KW(1469),KW(1442),KW(1473),KW(1158),KW(85),KW(86),KW(87),KW(1474),KW(1155)};
static constexpr std::string_view EKW_352[] = {KW(1475),KW(1476),KW(1477),KW(1143),KW(1478),KW(1479)};
static constexpr std::string_view EKW_353[] = {KW(1480),KW(1144),KW(1481),KW(1482),KW(1483),KW(1154)};
static constexpr std::string_view EKW_354[] = {KW(1484),KW(1147),KW(1481),KW(1482),KW(1483),KW(1148),KW(1155)};
static constexpr std::string_view EKW_355[] = {KW(1485),KW(1144),KW(1134),KW(1486),KW(1487),KW(1488),KW(1489),KW(424),KW(1490),KW(1024),KW(1491)};
static constexpr std::string_view EKW_356[] = {KW(1492),KW(1147),KW(1493),KW(1494),KW(1495)};
static constexpr std::string_view EKW_357[] = {KW(1496),KW(1497),KW(1498),KW(1499),KW(1500),KW(1144),KW(1154),KW(1024),KW(1501)};
static constexpr std::string_view EKW_358[] = {KW(1502),KW(1503),KW(1064),KW(1500),KW(1144),KW(1154),KW(1024),KW(1501),KW(1499)};
static constexpr std::string_view EKW_359[] = {KW(1504),KW(1503),KW(1064),KW(1147),KW(1499),KW(1155)};
static constexpr std::string_view EKW_360[] = {KW(1505),KW(1497),KW(1498),KW(1499),KW(1148),KW(1506),KW(1024)};
static constexpr std::string_view EKW_361[] = {KW(1507),KW(1499),KW(1498),KW(1506),KW(1144),KW(1154)};
static constexpr std::string_view EKW_362[] = {KW(1508),KW(1499),KW(1498),KW(1506),KW(1147),KW(1155)};
static constexpr std::string_view EKW_363[] = {KW(1509),KW(1132),KW(1147),KW(1510),KW(1511),KW(1155)};
static constexpr std::string_view EKW_364[] = {KW(1512),KW(1132),KW(1513),KW(1514),KW(799)};
static constexpr std::string_view EKW_365[] = {KW(1515),KW(1132),KW(1513),KW(1514),KW(799)};
static constexpr std::string_view EKW_366[] = {KW(1516),KW(1517),KW(1132),KW(1518),KW(1133),KW(1147),KW(1519),KW(1520),KW(1521),KW(1148),KW(1155)};
static constexpr std::string_view EKW_367[] = {KW(1522),KW(1523),KW(1310),KW(1524),KW(1133),KW(1147),KW(1519),KW(1520),KW(1517),KW(1155)};
static constexpr std::string_view EKW_368[] = {KW(1525),KW(1523),KW(1310),KW(1524),KW(1133),KW(1519),KW(1144),KW(1154),KW(1520),KW(1517)};
static constexpr std::string_view EKW_369[] = {KW(1526),KW(1523),KW(1310),KW(1524),KW(1133),KW(1519),KW(1520),KW(1517)};
static constexpr std::string_view EKW_370[] = {KW(1527),KW(82),KW(1528),KW(83),KW(1529),KW(700),KW(1),KW(85),KW(86),KW(1530),KW(87)};
static constexpr std::string_view EKW_371[] = {KW(1531),KW(1532),KW(1143),KW(1144),KW(1533),KW(1534),KW(1535),KW(426),KW(1154),KW(1536),KW(1537),KW(1538)};
static constexpr std::string_view EKW_372[] = {KW(1539),KW(1148),KW(1147),KW(1533),KW(1540),KW(1535),KW(426),KW(1541),KW(1542),KW(1155)};
static constexpr std::string_view EKW_373[] = {KW(1543),KW(1544),KW(1535),KW(426),KW(1545),KW(1542)};
static constexpr std::string_view EKW_374[] = {KW(1546),KW(1547),KW(86),KW(935),KW(1548),KW(1549),KW(1550),KW(1551)};
static constexpr std::string_view EKW_375[] = {KW(1552),KW(1143),KW(1144),KW(935),KW(1548),KW(1551),KW(86),KW(1154),KW(1550)};
static constexpr std::string_view EKW_376[] = {KW(1553),KW(1148),KW(1147),KW(935),KW(1549),KW(1551),KW(86),KW(1548),KW(1550),KW(1155)};
static constexpr std::string_view EKW_377[] = {KW(1554),KW(1547),KW(624),KW(1555),KW(591),KW(86),KW(1550),KW(1551),KW(1556)};
static constexpr std::string_view EKW_378[] = {KW(1557),KW(1143),KW(1144),KW(591),KW(624),KW(1555),KW(1548),KW(1551),KW(86),KW(1154),KW(1550),KW(1556)};
static constexpr std::string_view EKW_379[] = {KW(1558),KW(1148),KW(1147),KW(591),KW(624),KW(1555),KW(1549),KW(1551),KW(86),KW(1550),KW(1556),KW(1155)};
static constexpr std::string_view EKW_380[] = {KW(1559),KW(814),KW(86),KW(1560),KW(1561),KW(1562),KW(1563)};
static constexpr std::string_view EKW_381[] = {KW(1564),KW(1143),KW(1144),KW(1559),KW(1560),KW(86),KW(1154),KW(1563)};
static constexpr std::string_view EKW_382[] = {KW(1565),KW(1148),KW(1147),KW(1559),KW(1562),KW(86),KW(1561),KW(1563),KW(1155)};
static constexpr std::string_view EKW_383[] = {KW(85),KW(1528),KW(1566),KW(86),KW(1567),KW(1568),KW(1569)};
static constexpr std::string_view EKW_384[] = {KW(1570),KW(1143),KW(1144),KW(86),KW(1154),KW(1567),KW(1568)};
static constexpr std::string_view EKW_385[] = {KW(1571),KW(1148),KW(1147),KW(86),KW(1569),KW(1528),KW(1155)};
static constexpr std::string_view EKW_386[] = {KW(1572),KW(1573),KW(1574),KW(1575),KW(86),KW(1576)};
static constexpr std::string_view EKW_387[] = {KW(1577),KW(1143),KW(1144),KW(1575),KW(86),KW(1154),KW(1576)};
static constexpr std::string_view EKW_388[] = {KW(1578),KW(1148),KW(1147),KW(86),KW(1576),KW(1579),KW(1155)};
static constexpr std::string_view EKW_389[] = {KW(1580),KW(1581),KW(86),KW(1582),KW(1583),KW(1584),KW(1585),KW(1586)};
static constexpr std::string_view EKW_390[] = {KW(1585),KW(1143),KW(1144),KW(1587),KW(86),KW(1154),KW(1584)};
static constexpr std::string_view EKW_391[] = {KW(1584),KW(1148),KW(1147),KW(1586),KW(1588),KW(86),KW(1155)};
static constexpr std::string_view EKW_392[] = {KW(1589),KW(1566),KW(688),KW(86),KW(1590),KW(1591)};
static constexpr std::string_view EKW_393[] = {KW(1592),KW(1143),KW(1144),KW(688),KW(86),KW(1566),KW(1154),KW(1591)};
static constexpr std::string_view EKW_394[] = {KW(1593),KW(1148),KW(1147),KW(688),KW(86),KW(1566),KW(1591),KW(1155)};
static constexpr std::string_view EKW_395[] = {KW(1594),KW(1566),KW(1595),KW(1596),KW(1597),KW(86),KW(1598)};
static constexpr std::string_view EKW_396[] = {KW(1599),KW(1143),KW(1144),KW(1596),KW(86),KW(1154)};
static constexpr std::string_view EKW_397[] = {KW(1600),KW(1148),KW(1147),KW(1596),KW(86),KW(1155)};
static constexpr std::string_view EKW_398[] = {KW(1601),KW(405),KW(86),KW(1576),KW(1602)};
static constexpr std::string_view EKW_399[] = {KW(1603),KW(1143),KW(1144),KW(1575),KW(1576),KW(1604),KW(86),KW(1154)};
static constexpr std::string_view EKW_400[] = {KW(1605),KW(1148),KW(1147),KW(1576),KW(1604),KW(86),KW(1155)};
static constexpr std::string_view EKW_401[] = {KW(1606),KW(1607),KW(606),KW(85),KW(86),KW(87),KW(633)};
static constexpr std::string_view EKW_402[] = {KW(1608),KW(1609),KW(1),KW(261),KW(1610),KW(1611),KW(1612)};
static constexpr std::string_view EKW_403[] = {KW(1613),KW(1144),KW(1134),KW(1143),KW(261),KW(1),KW(1154),KW(1611),KW(1612)};
static constexpr std::string_view EKW_404[] = {KW(1614),KW(1147),KW(1135),KW(1148),KW(261),KW(1),KW(1611),KW(1612),KW(1155)};
static constexpr std::string_view EKW_405[] = {KW(1615),KW(1157),KW(1616),KW(1062),KW(1617),KW(1180),KW(1618),KW(1619)};
static constexpr std::string_view EKW_406[] = {KW(1620),KW(1144),KW(1134),KW(1143),KW(1616),KW(1062),KW(1154),KW(1619)};
static constexpr std::string_view EKW_407[] = {KW(1621),KW(1147),KW(1135),KW(1148),KW(1616),KW(1062),KW(1619),KW(1155)};
static constexpr std::string_view EKW_408[] = {KW(1622),KW(1623),KW(1624),KW(1625),KW(1626),KW(1627)};
static constexpr std::string_view EKW_409[] = {KW(1628),KW(1195),KW(1629),KW(1630),KW(1624),KW(1144),KW(1154),KW(1625),KW(1626)};
static constexpr std::string_view EKW_410[] = {KW(1631),KW(1195),KW(1629),KW(1630),KW(1148),KW(1147),KW(1624),KW(1625),KW(1626),KW(1155)};
static constexpr std::string_view EKW_411[] = {KW(1632),KW(1633),KW(38)};
static constexpr std::string_view EKW_412[] = {KW(1634),KW(1024),KW(38)};
static constexpr std::string_view EKW_413[] = {KW(1635),KW(1024),KW(38)};
static constexpr std::string_view EKW_414[] = {KW(1636),KW(1637),KW(1638)};
static constexpr std::string_view EKW_415[] = {KW(1639),KW(1637),KW(1144),KW(1154),KW(1638)};
static constexpr std::string_view EKW_416[] = {KW(1640),KW(1637),KW(1147),KW(1638),KW(1155)};
static constexpr std::string_view EKW_417[] = {KW(1641),KW(1047),KW(1642),KW(1643)};
static constexpr std::string_view EKW_418[] = {KW(1644),KW(1047),KW(1642),KW(1643),KW(1144),KW(1154)};
static constexpr std::string_view EKW_419[] = {KW(1645),KW(1642),KW(1047),KW(1147),KW(1643),KW(1155)};
static constexpr std::string_view EKW_420[] = {KW(1646),KW(1047)};
static constexpr std::string_view EKW_421[] = {KW(1647),KW(1047),KW(1648)};
static constexpr std::string_view EKW_422[] = {KW(1649),KW(1047),KW(1648)};
static constexpr std::string_view EKW_423[] = {KW(1650),KW(679),KW(1083),KW(119)};
static constexpr std::string_view EKW_424[] = {KW(1651),KW(1626),KW(1626),KW(1652),KW(679)};
static constexpr std::string_view EKW_425[] = {KW(1653),KW(679),KW(1083),KW(1144),KW(1154),KW(119)};
static constexpr std::string_view EKW_426[] = {KW(1654),KW(1652),KW(679),KW(1626),KW(1655)};
static constexpr std::string_view EKW_427[] = {KW(1656),KW(679),KW(1083),KW(1147),KW(119),KW(1155)};
static constexpr std::string_view EKW_428[] = {KW(1657),KW(1655),KW(1652),KW(679)};
static constexpr std::string_view EKW_429[] = {KW(1658),KW(1659),KW(1083)};
static constexpr std::string_view EKW_430[] = {KW(1660),KW(1083),KW(1659)};
static constexpr std::string_view EKW_431[] = {KW(1661),KW(1659),KW(1083),KW(1144),KW(1154)};
static constexpr std::string_view EKW_432[] = {KW(1662),KW(1659),KW(1083),KW(1663)};
static constexpr std::string_view EKW_433[] = {KW(1664),KW(1659),KW(1083),KW(1147),KW(1155)};
static constexpr std::string_view EKW_434[] = {KW(1665),KW(1663),KW(1083),KW(1659)};
static constexpr std::string_view EKW_435[] = {KW(1666),KW(1659),KW(1083)};
static constexpr std::string_view EKW_436[] = {KW(1667),KW(1663),KW(1083),KW(1659)};
static constexpr std::string_view EKW_437[] = {KW(1668),KW(1659),KW(1083),KW(1144),KW(1154)};
static constexpr std::string_view EKW_438[] = {KW(1669),KW(1663),KW(1083),KW(1659)};
static constexpr std::string_view EKW_439[] = {KW(1670),KW(1659),KW(1083),KW(1147),KW(1155)};
static constexpr std::string_view EKW_440[] = {KW(1671),KW(1659),KW(1663),KW(1083)};
static constexpr std::string_view EKW_441[] = {KW(1672),KW(1623),KW(38),KW(1673),KW(1674),KW(1675),KW(1676),KW(824)};
static constexpr std::string_view EKW_442[] = {KW(1677),KW(1143),KW(1602),KW(38),KW(1678),KW(835),KW(1144),KW(1674),KW(1154),KW(1679),KW(1676),KW(824)};
static constexpr std::string_view EKW_443[] = {KW(1680),KW(1148),KW(1602),KW(38),KW(1678),KW(835),KW(1147),KW(1134),KW(1674),KW(1679),KW(1676),KW(1155),KW(824)};
static constexpr std::string_view EKW_444[] = {KW(1681),KW(38),KW(1682)};
static constexpr std::string_view EKW_445[] = {KW(1683),KW(38),KW(1682)};
static constexpr std::string_view EKW_446[] = {KW(1684),KW(1682),KW(38)};
static constexpr std::string_view EKW_447[] = {KW(1685),KW(1147),KW(1135),KW(1148),KW(1686),KW(1687),KW(1688),KW(1689),KW(381),KW(1690),KW(1155)};
static constexpr std::string_view EKW_448[] = {KW(1691),KW(1144),KW(1134),KW(1686),KW(1688),KW(1687),KW(1692),KW(1154)};
static constexpr std::string_view EKW_449[] = {KW(1693),KW(1501),KW(1335),KW(1694),KW(1695),KW(1696),KW(1134),KW(1697),KW(1698),KW(1144),KW(1154),KW(1024),KW(959),KW(1699)};
static constexpr std::string_view EKW_450[] = {KW(1700),KW(1701),KW(1702),KW(1688),KW(1703),KW(1091),KW(1704),KW(1705),KW(1155)};
static constexpr std::string_view EKW_451[] = {KW(1706),KW(1144),KW(1707),KW(1154),KW(1708),KW(1688),KW(1703),KW(1091),KW(1143),KW(1704),KW(1705)};
static constexpr std::string_view EKW_452[] = {KW(1709),KW(1147),KW(1707),KW(1155),KW(1181),KW(1688),KW(1703),KW(1091),KW(1704),KW(1710),KW(1705)};
static constexpr std::string_view EKW_453[] = {KW(1711),KW(1609),KW(1612),KW(1712),KW(1713),KW(254),KW(1714)};
static constexpr std::string_view EKW_454[] = {KW(1715),KW(1144),KW(1143),KW(1612),KW(1716),KW(1713),KW(1712),KW(1154),KW(254),KW(1714)};
static constexpr std::string_view EKW_455[] = {KW(1717),KW(1147),KW(1148),KW(1612),KW(1716),KW(1713),KW(1712),KW(254),KW(1714),KW(1155)};
static constexpr std::string_view EKW_456[] = {KW(1718),KW(1719),KW(1720),KW(1721),KW(223)};
static constexpr std::string_view EKW_457[] = {KW(1722),KW(1723),KW(1724),KW(1143),KW(1144),KW(223),KW(1720),KW(1721),KW(1154)};
static constexpr std::string_view EKW_458[] = {KW(1725),KW(1723),KW(1724),KW(1148),KW(1147),KW(223),KW(1720),KW(1721),KW(1155)};
static constexpr std::string_view EKW_459[] = {KW(1726),KW(1723),KW(1727),KW(1728),KW(1729)};
static constexpr std::string_view EKW_460[] = {KW(1730),KW(660),KW(1731),KW(1732),KW(1733),KW(939),KW(1734),KW(1678),KW(1735)};
static constexpr std::string_view EKW_461[] = {KW(1736),KW(1723),KW(1737),KW(1738),KW(1739)};
static constexpr std::string_view EKW_462[] = {KW(1740),KW(1723),KW(1737),KW(1739),KW(1738),KW(1741),KW(1742)};
static constexpr std::string_view EKW_463[] = {KW(1743),KW(1723),KW(1335),KW(1744),KW(1745),KW(1013),KW(1746)};
static constexpr std::string_view EKW_464[] = {KW(1747),KW(1719),KW(1744),KW(1013),KW(1746),KW(1144),KW(1154)};
static constexpr std::string_view EKW_465[] = {KW(1748),KW(1723),KW(1335),KW(1148),KW(1147),KW(1744),KW(1013),KW(1746),KW(1155)};
static constexpr std::string_view EKW_466[] = {KW(1749),KW(1719),KW(1581),KW(1750),KW(1751)};
static constexpr std::string_view EKW_467[] = {KW(1752),KW(1723),KW(1753),KW(1581),KW(440),KW(441),KW(1144),KW(1154),KW(1751)};
static constexpr std::string_view EKW_468[] = {KW(1754),KW(1723),KW(1753),KW(1581),KW(440),KW(441),KW(1148),KW(1147),KW(1751),KW(1155)};
static constexpr std::string_view EKW_469[] = {KW(1755),KW(1719),KW(1623),KW(1756),KW(1757),KW(1758)};
static constexpr std::string_view EKW_470[] = {KW(1759),KW(1723),KW(1724),KW(819),KW(1760),KW(1144),KW(1154),KW(1757),KW(1758)};
static constexpr std::string_view EKW_471[] = {KW(1761),KW(1723),KW(1724),KW(819),KW(1760),KW(1148),KW(1147),KW(1757),KW(1758),KW(1155)};
static constexpr std::string_view EKW_472[] = {KW(1762),KW(1719),KW(1763),KW(1764),KW(1765)};
static constexpr std::string_view EKW_473[] = {KW(1766),KW(1723),KW(38),KW(1195),KW(1767),KW(819),KW(440),KW(1144),KW(1154),KW(1764),KW(1765)};
static constexpr std::string_view EKW_474[] = {KW(1768),KW(1723),KW(38),KW(1195),KW(1767),KW(819),KW(440),KW(1148),KW(1147),KW(1764),KW(1765),KW(1155)};
static constexpr std::string_view EKW_475[] = {KW(1769),KW(1723),KW(1195),KW(1770),KW(1771)};
static constexpr std::string_view EKW_476[] = {KW(1772),KW(1719),KW(1770),KW(1144),KW(1154),KW(1771)};
static constexpr std::string_view EKW_477[] = {KW(1773),KW(1723),KW(1195),KW(1148),KW(1147),KW(1770),KW(1771),KW(1155)};
static constexpr std::string_view EKW_478[] = {KW(1774),KW(1723),KW(1775),KW(38),KW(1776),KW(1777),KW(1778),KW(1779),KW(1780),KW(824)};
static constexpr std::string_view EKW_479[] = {KW(1781),KW(1719),KW(1777),KW(1778),KW(1144),KW(1154),KW(1779),KW(1780),KW(824)};
static constexpr std::string_view EKW_480[] = {KW(1782),KW(1723),KW(1775),KW(38),KW(1148),KW(1147),KW(1777),KW(1778),KW(1779),KW(1780),KW(1155),KW(824)};
static constexpr std::string_view EKW_481[] = {KW(1783),KW(1784),KW(1785),KW(1786),KW(1719),KW(1623),KW(1787)};
static constexpr std::string_view EKW_482[] = {KW(1788),KW(1784),KW(1785),KW(1786),KW(1723),KW(38),KW(1789),KW(1787),KW(1144),KW(1154)};
static constexpr std::string_view EKW_483[] = {KW(1790),KW(1784),KW(1785),KW(1786),KW(1723),KW(38),KW(1789),KW(1148),KW(1147),KW(1787),KW(1155)};
static constexpr std::string_view EKW_484[] = {KW(1791),KW(1784),KW(1785),KW(1786),KW(1719),KW(1623),KW(1787),KW(1792)};
static constexpr std::string_view EKW_485[] = {KW(1793),KW(1784),KW(1785),KW(1786),KW(1794),KW(1723),KW(1195),KW(1678),KW(1787),KW(1792),KW(1144),KW(1154)};
static constexpr std::string_view EKW_486[] = {KW(1795),KW(1784),KW(1785),KW(1786),KW(1794),KW(1723),KW(1195),KW(1678),KW(1148),KW(1147),KW(1787),KW(1792),KW(1155)};
static constexpr std::string_view EKW_487[] = {KW(1796),KW(1719),KW(1797),KW(1798),KW(1799),KW(1800),KW(1801)};
static constexpr std::string_view EKW_488[] = {KW(1802),KW(1801),KW(1798),KW(1799),KW(1144),KW(1154)};
static constexpr std::string_view EKW_489[] = {KW(1803),KW(1801),KW(1798),KW(1799),KW(1147),KW(1155)};
static constexpr std::string_view EKW_490[] = {KW(1804),KW(1719),KW(1805),KW(1806),KW(1807)};
static constexpr std::string_view EKW_491[] = {KW(1808),KW(1723),KW(1807),KW(1144),KW(1143),KW(1805),KW(1806)};
static constexpr std::string_view EKW_492[] = {KW(1809),KW(1723),KW(1807),KW(1147),KW(1148),KW(1805),KW(1806)};
static constexpr std::string_view EKW_493[] = {KW(1810),KW(1719)};
static constexpr std::string_view EKW_494[] = {KW(1811),KW(1723),KW(1763),KW(1144),KW(1154)};
static constexpr std::string_view EKW_495[] = {KW(1812),KW(1723),KW(1763),KW(1147),KW(1155)};
static constexpr std::string_view EKW_496[] = {KW(1813),KW(1719),KW(1744)};
static constexpr std::string_view EKW_497[] = {KW(1814),KW(1723),KW(1744),KW(1144),KW(1154)};
static constexpr std::string_view EKW_498[] = {KW(1815),KW(1723),KW(1744),KW(1147),KW(1155)};
static constexpr std::string_view EKW_499[] = {KW(1816),KW(1817),KW(1818),KW(1819),KW(1820),KW(1821),KW(1822)};
static constexpr std::string_view EKW_500[] = {KW(1823),KW(1819),KW(1818),KW(1822),KW(1821),KW(1820),KW(1144),KW(1154)};
static constexpr std::string_view EKW_501[] = {KW(1824),KW(1819),KW(1818),KW(1822),KW(1821),KW(1147),KW(1820),KW(1155)};
static constexpr std::string_view EKW_502[] = {KW(1825),KW(1826),KW(1827),KW(1828),KW(1829)};
static constexpr std::string_view EKW_503[] = {KW(1830),KW(1143),KW(1144),KW(1827),KW(1829),KW(1828),KW(1831),KW(1832),KW(1154)};
static constexpr std::string_view EKW_504[] = {KW(1833),KW(1148),KW(1147),KW(1827),KW(1829),KW(1828),KW(1831),KW(1832),KW(1155)};
static constexpr std::string_view EKW_505[] = {KW(1834),KW(1835),KW(253),KW(1836),KW(1837),KW(1838),KW(34)};
static constexpr std::string_view EKW_506[] = {KW(1839),KW(1840),KW(326),KW(1841),KW(1842),KW(330),KW(865)};
static constexpr std::string_view EKW_507[] = {KW(1843),KW(1844),KW(1497),KW(1845),KW(1140),KW(879),KW(1846),KW(1141),KW(229),KW(1847)};
static constexpr std::string_view EKW_508[] = {KW(1848),KW(1849),KW(1844),KW(1497),KW(89),KW(17),KW(1147),KW(1710),KW(1195),KW(1845),KW(879),KW(1846),KW(1850),KW(1851),KW(562),KW(724),KW(1148)};
static constexpr std::string_view EKW_509[] = {KW(1852),KW(1849),KW(1710),KW(1195),KW(89),KW(1845),KW(1853),KW(17),KW(90),KW(91),KW(1498),KW(1497),KW(1147),KW(879),KW(1854),KW(1846),KW(1144),KW(1154),KW(232),KW(1155)};
static constexpr std::string_view EKW_510[] = {KW(1855),KW(1849),KW(1497),KW(89),KW(17),KW(1856),KW(1844),KW(1710),KW(1195),KW(1845),KW(1857),KW(879),KW(1846),KW(1851),KW(1144),KW(1143),KW(562),KW(724)};
static constexpr std::string_view EKW_511[] = {KW(105),KW(1849),KW(91),KW(89),KW(17),KW(1853),KW(1498),KW(1497),KW(1858),KW(1147),KW(1140),KW(97),KW(107),KW(1144),KW(1143),KW(1154),KW(229),KW(698),KW(1148),KW(1155)};
static constexpr std::string_view EKW_512[] = {KW(1859),KW(89),KW(1497),KW(1858),KW(1147),KW(97),KW(107),KW(1144),KW(1154),KW(698),KW(1155)};
static constexpr std::string_view EKW_513[] = {KW(1860),KW(1849),KW(91),KW(89),KW(17),KW(1853),KW(1498),KW(1497),KW(1858),KW(1857),KW(97),KW(107),KW(1851),KW(1144),KW(1154),KW(562),KW(698),KW(724)};
static constexpr std::string_view EKW_514[] = {KW(1861),KW(1849),KW(91),KW(89),KW(17),KW(1853),KW(1498),KW(1497),KW(1858),KW(1147),KW(97),KW(107),KW(1850),KW(1851),KW(562),KW(698),KW(724),KW(1155)};
static constexpr std::string_view EKW_515[] = {KW(1862),KW(1849),KW(89),KW(17),KW(90),KW(1195),KW(1853),KW(91),KW(1498),KW(1147),KW(1140),KW(1863),KW(1144),KW(1143),KW(1154),KW(229),KW(698),KW(1148),KW(1155)};
static constexpr std::string_view EKW_516[] = {KW(1864),KW(89),KW(1147),KW(1144),KW(1154),KW(698),KW(1155)};
static constexpr std::string_view EKW_517[] = {KW(1865),KW(1849),KW(89),KW(17),KW(90),KW(1195),KW(1853),KW(91),KW(1498),KW(1857),KW(1851),KW(1144),KW(1154),KW(562),KW(698),KW(724)};
static constexpr std::string_view EKW_518[] = {KW(1866),KW(1849),KW(89),KW(17),KW(90),KW(1195),KW(1853),KW(91),KW(1498),KW(1147),KW(1850),KW(1851),KW(562),KW(698),KW(724),KW(1155)};
static constexpr std::string_view EKW_519[] = {KW(1867),KW(89),KW(1161),KW(1521),KW(1868)};
static constexpr std::string_view EKW_520[] = {KW(1869),KW(536),KW(1870),KW(1710),KW(1195),KW(1133),KW(1871),KW(1161),KW(1147),KW(1144),KW(1154),KW(1521),KW(1155)};
static constexpr std::string_view EKW_521[] = {KW(1872),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1161),KW(1147),KW(1144),KW(1154),KW(1521),KW(1868),KW(1155)};
static constexpr std::string_view EKW_522[] = {KW(1874),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1161),KW(1147),KW(1144),KW(1154),KW(1521),KW(1875),KW(724),KW(1155)};
static constexpr std::string_view EKW_523[] = {KW(1876),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1877),KW(1161),KW(1147),KW(1144),KW(1154),KW(1521),KW(724),KW(1155)};
static constexpr std::string_view EKW_524[] = {KW(1878),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1161),KW(1879),KW(1857),KW(1851),KW(1144),KW(1154),KW(562),KW(1868),KW(724)};
static constexpr std::string_view EKW_525[] = {KW(1880),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1161),KW(1879),KW(1857),KW(1851),KW(1144),KW(1154),KW(562),KW(724)};
static constexpr std::string_view EKW_526[] = {KW(1881),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1161),KW(1879),KW(1857),KW(1851),KW(1144),KW(1154),KW(562),KW(1868),KW(724)};
static constexpr std::string_view EKW_527[] = {KW(1882),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1161),KW(1879),KW(1857),KW(1851),KW(1144),KW(1154),KW(562),KW(1875),KW(724)};
static constexpr std::string_view EKW_528[] = {KW(1883),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1877),KW(1161),KW(1879),KW(1857),KW(1851),KW(1144),KW(1154),KW(562),KW(724)};
static constexpr std::string_view EKW_529[] = {KW(1884),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1147),KW(1850),KW(1851),KW(1521),KW(1885),KW(562),KW(1868),KW(724),KW(1155)};
static constexpr std::string_view EKW_530[] = {KW(1886),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1147),KW(1850),KW(1851),KW(1521),KW(1885),KW(562),KW(724),KW(1155)};
static constexpr std::string_view EKW_531[] = {KW(1887),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1147),KW(1850),KW(1851),KW(1521),KW(1885),KW(562),KW(1868),KW(724),KW(1155)};
static constexpr std::string_view EKW_532[] = {KW(1888),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1147),KW(1850),KW(1851),KW(1521),KW(1885),KW(562),KW(1875),KW(724),KW(1155)};
static constexpr std::string_view EKW_533[] = {KW(1889),KW(536),KW(1870),KW(1710),KW(1195),KW(1873),KW(1133),KW(1877),KW(1147),KW(1850),KW(1851),KW(1521),KW(1885),KW(562),KW(724),KW(1155)};
static constexpr std::string_view EKW_534[] = {KW(1890),KW(536),KW(1891),KW(1710),KW(1195),KW(1133),KW(1161),KW(1144),KW(1154),KW(1868)};
static constexpr std::string_view EKW_535[] = {KW(1892),KW(536),KW(1891),KW(1710),KW(1195),KW(1873),KW(1133),KW(1161),KW(1144),KW(1154),KW(1875),KW(724)};
static constexpr std::string_view EKW_536[] = {KW(1893),KW(536),KW(1891),KW(1710),KW(1195),KW(1133),KW(1871),KW(1161),KW(1147),KW(1144)};
static constexpr std::string_view EKW_537[] = {KW(1894),KW(536),KW(1891),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1161),KW(1144),KW(1154),KW(1868)};
static constexpr std::string_view EKW_538[] = {KW(1895),KW(536),KW(1891),KW(1710),KW(1195),KW(1873),KW(1133),KW(1877),KW(1161),KW(1147),KW(1144),KW(724)};
static constexpr std::string_view EKW_539[] = {KW(1896),KW(536),KW(1891),KW(1710),KW(1195),KW(1133),KW(1147),KW(1521),KW(1868),KW(1155)};
static constexpr std::string_view EKW_540[] = {KW(1897),KW(536),KW(1891),KW(1710),KW(1195),KW(1873),KW(1133),KW(1147),KW(1521),KW(1875),KW(724),KW(1155)};
static constexpr std::string_view EKW_541[] = {KW(1898),KW(536),KW(1891),KW(1710),KW(1195),KW(1133),KW(1871),KW(1147),KW(1521),KW(1155)};
static constexpr std::string_view EKW_542[] = {KW(1899),KW(536),KW(1891),KW(1710),KW(1195),KW(1873),KW(1133),KW(1871),KW(1147),KW(1144),KW(1521),KW(1868)};
static constexpr std::string_view EKW_543[] = {KW(1900),KW(536),KW(1891),KW(1710),KW(1195),KW(1873),KW(1133),KW(1877),KW(1147),KW(1521),KW(724),KW(1155)};
static constexpr std::string_view EKW_544[] = {KW(1901),KW(1902),KW(1024),KW(1195),KW(1903),KW(1904),KW(841),KW(1),KW(1905),KW(1906),KW(1907),KW(1908),KW(1909)};
static constexpr std::string_view EKW_545[] = {KW(1910),KW(1902),KW(1024),KW(1195),KW(1911)};
static constexpr std::string_view EKW_546[] = {KW(1912),KW(1902),KW(1024),KW(1195),KW(1913),KW(1914),KW(1915),KW(1710),KW(1916),KW(1917),KW(724),KW(1918),KW(1919)};
static constexpr std::string_view EKW_547[] = {KW(1920),KW(1921),KW(868),KW(871),KW(167),KW(1049)};
static constexpr std::string_view EKW_548[] = {KW(1922),KW(536),KW(1870),KW(1133),KW(1923),KW(1162),KW(1161),KW(1521),KW(1710),KW(1195),KW(1134),KW(1147),KW(1144),KW(1143),KW(1154),KW(1148),KW(1155)};
static constexpr std::string_view EKW_549[] = {KW(1924),KW(1925),KW(1870)};
static constexpr std::string_view EKW_550[] = {KW(1926),KW(1873),KW(1870)};
static constexpr std::string_view EKW_551[] = {KW(1927),KW(1891),KW(1925)};
static constexpr std::string_view EKW_552[] = {KW(1928),KW(1891),KW(1873)};
static constexpr std::string_view EKW_553[] = {KW(1929),KW(1629),KW(1930),KW(1602),KW(441),KW(602),KW(1931),KW(1932),KW(1933),KW(1934),KW(1935)};
static constexpr std::string_view EKW_554[] = {KW(1936)};
static constexpr std::string_view EKW_555[] = {KW(1937),KW(660),KW(678),KW(1938),KW(261)};
static constexpr std::string_view EKW_556[] = {KW(677),KW(660),KW(678),KW(1939),KW(1938),KW(146)};
static constexpr std::string_view EKW_557[] = {KW(1940),KW(660),KW(678),KW(1938)};
static constexpr std::string_view EKW_558[] = {KW(1941),KW(660),KW(1942)};
static constexpr std::string_view EKW_559[] = {KW(1943),KW(660),KW(1944),KW(678),KW(1945),KW(504),KW(1946),KW(1947)};
static constexpr std::string_view EKW_560[] = {KW(621),KW(660),KW(678),KW(1944),KW(1948),KW(1946),KW(1947),KW(1949),KW(1950)};
static constexpr std::string_view EKW_561[] = {KW(1951),KW(660),KW(679),KW(1083),KW(18),KW(1952)};
static constexpr std::string_view EKW_562[] = {KW(1953),KW(679),KW(660),KW(1083),KW(1954)};
static constexpr std::string_view EKW_563[] = {KW(1955),KW(621),KW(660),KW(1956),KW(678),KW(1946),KW(1957),KW(1958),KW(1959)};
static constexpr std::string_view EKW_564[] = {KW(1960),KW(660),KW(678),KW(157),KW(1)};
static constexpr std::string_view EKW_565[] = {KW(1961),KW(660),KW(678),KW(1)};
static constexpr std::string_view EKW_566[] = {KW(1962),KW(660),KW(678),KW(1963),KW(1),KW(1964)};
static constexpr std::string_view EKW_567[] = {KW(1965),KW(660),KW(1966),KW(678),KW(1946),KW(1967),KW(1968)};
static constexpr std::string_view EKW_568[] = {KW(1969),KW(660),KW(1966),KW(1946),KW(661),KW(1970),KW(1971),KW(1972),KW(1973)};
static constexpr std::string_view EKW_569[] = {KW(1974),KW(1975),KW(939),KW(605),KW(1946),KW(1976)};
static constexpr std::string_view EKW_570[] = {KW(1977),KW(660),KW(678),KW(1),KW(1978),KW(1152)};
static constexpr std::string_view EKW_571[] = {KW(1979),KW(660),KW(1969),KW(603),KW(157),KW(678),KW(1980),KW(1246)};
static constexpr std::string_view EKW_572[] = {KW(1981),KW(660),KW(678),KW(1980),KW(1982),KW(1983)};
static constexpr std::string_view EKW_573[] = {KW(1984),KW(660),KW(678),KW(1985),KW(1986)};
static constexpr std::string_view EKW_574[] = {KW(1987),KW(660),KW(1988),KW(678),KW(261)};
static constexpr std::string_view EKW_575[] = {KW(1989),KW(1990),KW(1991),KW(1992),KW(439)};
static constexpr std::string_view EKW_576[] = {KW(1993),KW(1994),KW(1995)};
static constexpr std::string_view EKW_577[] = {KW(1996),KW(660),KW(1997),KW(939),KW(1998),KW(1999),KW(1735),KW(1679),KW(2000)};
static constexpr std::string_view EKW_578[] = {KW(2001),KW(660),KW(678),KW(1607),KW(1)};
static constexpr std::string_view EKW_579[] = {KW(2002),KW(660),KW(678),KW(2003),KW(2004),KW(1),KW(2005)};
static constexpr std::string_view EKW_580[] = {KW(2006),KW(660),KW(678),KW(590),KW(2007),KW(2008),KW(2009),KW(2010)};
static constexpr std::string_view EKW_581[] = {KW(2011),KW(2012),KW(2013),KW(2014),KW(2015)};
static constexpr std::string_view EKW_582[] = {KW(2016),KW(2017),KW(2012),KW(660),KW(678),KW(2018),KW(1520),KW(3)};
static constexpr std::string_view EKW_583[] = {KW(2012),KW(660),KW(2019),KW(2017),KW(2020),KW(2021),KW(2022),KW(2023),KW(2024),KW(1152)};
static constexpr std::string_view EKW_584[] = {KW(2025),KW(660),KW(678),KW(2012),KW(2019),KW(1971)};
static constexpr std::string_view EKW_585[] = {KW(2019),KW(2017),KW(2012),KW(660),KW(678),KW(2018),KW(1520),KW(2026),KW(2027)};
static constexpr std::string_view EKW_586[] = {KW(2028),KW(660),KW(2029),KW(678),KW(261)};
static constexpr std::string_view EKW_587[] = {KW(2030),KW(660),KW(678),KW(2031),KW(2032),KW(2033)};
static constexpr std::string_view EKW_588[] = {KW(2034),KW(660),KW(678),KW(2030),KW(2035),KW(157)};
static constexpr std::string_view EKW_589[] = {KW(2036),KW(660),KW(2029),KW(1),KW(2037)};
static constexpr std::string_view EKW_590[] = {KW(2038),KW(660),KW(2039),KW(678),KW(2040),KW(1144),KW(1152)};
static constexpr std::string_view EKW_591[] = {KW(2041),KW(660),KW(678),KW(2042),KW(2043),KW(1147),KW(2044),KW(2039)};
static constexpr std::string_view EKW_592[] = {KW(2045),KW(660),KW(678),KW(2046),KW(1152)};
static constexpr std::string_view EKW_593[] = {KW(2047),KW(660),KW(34),KW(2048),KW(2049),KW(2050),KW(998),KW(2051),KW(154)};
static constexpr std::string_view EKW_594[] = {KW(2052),KW(660),KW(678),KW(34),KW(2048),KW(2049),KW(1487),KW(2053),KW(998)};
static constexpr std::string_view EKW_595[] = {KW(2054),KW(660),KW(678),KW(2055),KW(2056),KW(2057),KW(2042)};
static constexpr std::string_view EKW_596[] = {KW(2058),KW(660),KW(678),KW(2059),KW(2004),KW(1)};
static constexpr std::string_view EKW_597[] = {KW(2060),KW(660),KW(678),KW(304),KW(2061),KW(1938)};
static constexpr std::string_view EKW_598[] = {KW(2062),KW(2060),KW(2063),KW(2064),KW(2065),KW(153),KW(2066),KW(2067)};
static constexpr std::string_view EKW_599[] = {KW(2068),KW(660),KW(678),KW(429),KW(2069)};
static constexpr std::string_view EKW_600[] = {KW(2070),KW(660),KW(678),KW(2071)};
static constexpr std::string_view EKW_601[] = {KW(2072),KW(660),KW(678),KW(2073),KW(2074)};
static constexpr std::string_view EKW_602[] = {KW(2075),KW(660),KW(678),KW(2074),KW(2076),KW(2077),KW(2078)};
static constexpr std::string_view EKW_603[] = {KW(2079),KW(660),KW(2075),KW(2074)};
static constexpr std::string_view EKW_604[] = {KW(2080),KW(660),KW(678),KW(1),KW(1946)};
static constexpr std::string_view EKW_605[] = {KW(2081),KW(660),KW(678),KW(1946),KW(2082),KW(814),KW(1707),KW(2083)};
static constexpr std::string_view EKW_606[] = {KW(2084),KW(660),KW(678),KW(1946),KW(814),KW(2082),KW(1707),KW(2085)};
static constexpr std::string_view EKW_607[] = {KW(2086),KW(660),KW(678),KW(2074),KW(2087)};
static constexpr std::string_view EKW_608[] = {KW(2088),KW(660),KW(2074),KW(2089)};
static constexpr std::string_view EKW_609[] = {KW(2090),KW(660),KW(678),KW(2091),KW(1)};
static constexpr std::string_view EKW_610[] = {KW(2092),KW(660),KW(678),KW(679),KW(1572),KW(2093)};
static constexpr std::string_view EKW_611[] = {KW(2094),KW(660),KW(678),KW(157),KW(1),KW(2095)};
static constexpr std::string_view EKW_612[] = {KW(2096),KW(660),KW(2097),KW(1),KW(119)};
static constexpr std::string_view EKW_613[] = {KW(2098),KW(660),KW(678),KW(2094),KW(1),KW(2099)};
static constexpr std::string_view EKW_614[] = {KW(2100),KW(660),KW(678),KW(1)};
static constexpr std::string_view EKW_615[] = {KW(2101),KW(660),KW(2102),KW(2103)};
static constexpr std::string_view EKW_616[] = {KW(2104),KW(660),KW(2105),KW(145)};
static constexpr std::string_view EKW_617[] = {KW(2106),KW(660),KW(2107),KW(2108)};
static constexpr std::string_view EKW_618[] = {KW(2109),KW(660),KW(678),KW(2110),KW(2111),KW(2112),KW(2099),KW(1696),KW(2113)};
static constexpr std::string_view EKW_619[] = {KW(2114),KW(660),KW(678),KW(2115),KW(2116)};
static constexpr std::string_view EKW_620[] = {KW(2117),KW(660),KW(1930),KW(1929),KW(621),KW(1969),KW(1946),KW(1629),KW(1967),KW(1934),KW(504)};
static constexpr std::string_view EKW_621[] = {KW(2118),KW(660),KW(961),KW(2119),KW(157)};
static constexpr std::string_view EKW_622[] = {KW(2120),KW(660),KW(2121),KW(678),KW(961),KW(2122)};
static constexpr std::string_view EKW_623[] = {KW(2123),KW(660),KW(678),KW(2120),KW(961),KW(2124),KW(2125)};
static constexpr std::string_view EKW_624[] = {KW(2126),KW(660),KW(2120),KW(2127),KW(2128),KW(1132),KW(961)};
static constexpr std::string_view EKW_625[] = {KW(2129),KW(660),KW(2120),KW(961),KW(1183)};
static constexpr std::string_view EKW_626[] = {KW(2130),KW(660),KW(2120),KW(1132),KW(961),KW(2131),KW(2132)};
static constexpr std::string_view EKW_627[] = {KW(961),KW(660),KW(678),KW(1394),KW(2133),KW(2082)};
static constexpr std::string_view EKW_628[] = {KW(2134),KW(660),KW(678),KW(961)};
static constexpr std::string_view EKW_629[] = {KW(2135),KW(660),KW(961),KW(1394),KW(930)};
static constexpr std::string_view EKW_630[] = {KW(2136),KW(660),KW(678),KW(961),KW(2137)};
static constexpr std::string_view EKW_631[] = {KW(111),KW(660),KW(678),KW(961),KW(2138)};
static constexpr std::string_view EKW_632[] = {KW(2139),KW(660),KW(678),KW(961),KW(2140),KW(2141),KW(2142)};
static constexpr std::string_view EKW_633[] = {KW(2143),KW(660),KW(678),KW(961),KW(2144),KW(2145)};
static constexpr std::string_view EKW_634[] = {KW(2146),KW(660),KW(961),KW(2064),KW(2065),KW(153),KW(2147),KW(2148)};
static constexpr std::string_view EKW_635[] = {KW(2149),KW(961),KW(1394),KW(2150),KW(2151),KW(2152)};
static constexpr std::string_view EKW_636[] = {KW(2153),KW(660),KW(2154),KW(2155)};
static constexpr std::string_view EKW_637[] = {KW(2156),KW(660),KW(678),KW(2157),KW(961),KW(2158),KW(79)};
static constexpr std::string_view EKW_638[] = {KW(2159),KW(660),KW(678),KW(961),KW(611),KW(841)};
static constexpr std::string_view EKW_639[] = {KW(2160),KW(81),KW(2161),KW(2162),KW(1394)};
static constexpr std::string_view EKW_640[] = {KW(2163),KW(2164)};
static constexpr std::string_view EKW_641[] = {KW(2165),KW(56),KW(2166),KW(2167),KW(2168)};
static constexpr std::string_view EKW_642[] = {KW(2169),KW(2170),KW(961),KW(2171),KW(2172)};
static constexpr std::string_view EKW_643[] = {KW(2173),KW(660),KW(678),KW(2174),KW(2175),KW(1)};
static constexpr std::string_view EKW_644[] = {KW(2176),KW(660),KW(678),KW(2177),KW(2178),KW(2179),KW(2180)};
static constexpr std::string_view EKW_645[] = {KW(2181),KW(660),KW(2103),KW(678),KW(2182),KW(2183)};
static constexpr std::string_view EKW_646[] = {KW(2178),KW(660),KW(678),KW(2177),KW(2184)};
static constexpr std::string_view EKW_647[] = {KW(2185),KW(660),KW(591),KW(678),KW(2186),KW(2187),KW(2188),KW(2189),KW(2190),KW(1152)};
static constexpr std::string_view EKW_648[] = {KW(2191),KW(660),KW(2192),KW(678),KW(1486),KW(364),KW(85),KW(261),KW(87)};
static constexpr std::string_view EKW_649[] = {KW(2193),KW(660),KW(2192),KW(678),KW(1486),KW(364),KW(85),KW(87)};
static constexpr std::string_view EKW_650[] = {KW(2194),KW(660),KW(678),KW(2195),KW(2196),KW(2197),KW(2198),KW(2064)};
static constexpr std::string_view EKW_651[] = {KW(2199),KW(660),KW(678),KW(2195),KW(2200),KW(2064),KW(2201)};
static constexpr std::string_view EKW_652[] = {KW(2202),KW(660),KW(678),KW(1581),KW(1753),KW(1246),KW(1)};
static constexpr std::string_view EKW_653[] = {KW(2203),KW(660),KW(678),KW(1581),KW(1753),KW(2204)};
static constexpr std::string_view EKW_654[] = {KW(2205),KW(660),KW(678),KW(2206),KW(1581),KW(1753),KW(2207),KW(2208),KW(441)};
static constexpr std::string_view EKW_655[] = {KW(2209),KW(660),KW(631),KW(1581),KW(1977)};
static constexpr std::string_view EKW_656[] = {KW(2206),KW(660),KW(1310),KW(678),KW(2210),KW(2211),KW(1152)};
static constexpr std::string_view EKW_657[] = {KW(2212),KW(660),KW(1764),KW(1753),KW(441),KW(2213),KW(386),KW(1183)};
static constexpr std::string_view EKW_658[] = {KW(2214),KW(660),KW(678),KW(1310),KW(1581),KW(1753),KW(2206),KW(2215),KW(2216)};
static constexpr std::string_view EKW_659[] = {KW(2217),KW(660),KW(678),KW(2206),KW(1581),KW(1753),KW(2218),KW(2208),KW(441),KW(2219),KW(119)};
static constexpr std::string_view EKW_660[] = {KW(2220),KW(660),KW(631),KW(1753),KW(1581),KW(678),KW(441)};
static constexpr std::string_view EKW_661[] = {KW(2221),KW(678),KW(1581),KW(441),KW(2222)};
static constexpr std::string_view EKW_662[] = {KW(2223),KW(1753),KW(1581),KW(2224)};
static constexpr std::string_view EKW_663[] = {KW(2225),KW(2226),KW(2227)};
static constexpr std::string_view EKW_664[] = {KW(2228),KW(660),KW(2229),KW(2230),KW(1152)};
static constexpr std::string_view EKW_665[] = {KW(2231),KW(660),KW(678),KW(2232),KW(2233),KW(2234)};
static constexpr std::string_view EKW_666[] = {KW(2235),KW(660),KW(1753),KW(678),KW(2234),KW(1310),KW(2236),KW(2237),KW(155)};
static constexpr std::string_view EKW_667[] = {KW(2238),KW(660),KW(678),KW(1753),KW(1581),KW(1310),KW(2239)};
static constexpr std::string_view EKW_668[] = {KW(2240),KW(1310),KW(2241),KW(2242)};
static constexpr std::string_view EKW_669[] = {KW(2243),KW(2103),KW(660),KW(2244),KW(2245),KW(2246)};
static constexpr std::string_view EKW_670[] = {KW(1029),KW(660),KW(2247),KW(678),KW(2248),KW(2249)};
static constexpr std::string_view EKW_671[] = {KW(2250),KW(660),KW(2247),KW(678),KW(2251),KW(2248)};
static constexpr std::string_view EKW_672[] = {KW(2252),KW(660),KW(2247),KW(678),KW(2250)};
static constexpr std::string_view EKW_673[] = {KW(2253),KW(660),KW(2247),KW(678),KW(2250),KW(2082),KW(2115),KW(2254),KW(2255)};
static constexpr std::string_view EKW_674[] = {KW(2256),KW(2247),KW(2250)};
static constexpr std::string_view EKW_675[] = {KW(2257),KW(660),KW(2247),KW(678),KW(2258),KW(2250),KW(2259)};
static constexpr std::string_view EKW_676[] = {KW(2260),KW(660),KW(2261),KW(2262),KW(2247),KW(2263)};
static constexpr std::string_view EKW_677[] = {KW(2264),KW(2247),KW(2265),KW(2266),KW(2267)};
static constexpr std::string_view EKW_678[] = {KW(2268),KW(660),KW(2269),KW(2247)};
static constexpr std::string_view EKW_679[] = {KW(2270),KW(660),KW(2247),KW(2269),KW(2271),KW(2272),KW(2273)};
static constexpr std::string_view EKW_680[] = {KW(2274),KW(660),KW(2269),KW(2275),KW(2276),KW(1152)};
static constexpr std::string_view EKW_681[] = {KW(2277),KW(660),KW(678),KW(2247),KW(2278),KW(344),KW(353),KW(2266),KW(2279)};
static constexpr std::string_view EKW_682[] = {KW(1394),KW(2247),KW(344),KW(2280),KW(2266),KW(2281)};
static constexpr std::string_view EKW_683[] = {KW(2251),KW(660),KW(2282),KW(2283),KW(2284)};
static constexpr std::string_view EKW_684[] = {KW(2285),KW(2286),KW(2287),KW(2288),KW(2279),KW(345),KW(2289),KW(346),KW(2290),KW(2291)};
static constexpr std::string_view EKW_685[] = {KW(2292),KW(2293),KW(678),KW(2082),KW(2294),KW(2295),KW(698)};
static constexpr std::string_view EKW_686[] = {KW(2296),KW(678),KW(2295),KW(2082),KW(2294),KW(713),KW(2297)};
static constexpr std::string_view EKW_687[] = {KW(2298),KW(1448),KW(2082),KW(2299),KW(2300),KW(2301),KW(2302),KW(2303),KW(2304),KW(759)};
static constexpr std::string_view EKW_688[] = {KW(2305),KW(2294),KW(2306),KW(1827),KW(2307),KW(1482),KW(2308),KW(2309),KW(2310)};
static constexpr std::string_view EKW_689[] = {KW(2311),KW(2294),KW(729),KW(2312),KW(2295)};
static constexpr std::string_view EKW_690[] = {KW(2313),KW(2293),KW(91),KW(89),KW(2082),KW(2294),KW(2295),KW(381)};
static constexpr std::string_view EKW_691[] = {KW(2314),KW(2295),KW(678),KW(2294),KW(2313),KW(405),KW(2315)};
static constexpr std::string_view EKW_692[] = {KW(2316),KW(2295),KW(2317),KW(2293),KW(441),KW(2294)};
static constexpr std::string_view EKW_693[] = {KW(2318),KW(678),KW(2295),KW(2319),KW(2294),KW(449),KW(1183)};
static constexpr std::string_view EKW_694[] = {KW(2299),KW(678),KW(2293),KW(1183),KW(2320),KW(2321),KW(2294),KW(2295)};
static constexpr std::string_view EKW_695[] = {KW(2322),KW(2293),KW(2295),KW(678),KW(440),KW(2082),KW(2294)};
static constexpr std::string_view EKW_696[] = {KW(2323),KW(2294),KW(2324)};
static constexpr std::string_view EKW_697[] = {KW(2325),KW(2295),KW(678),KW(2326),KW(2327),KW(2082),KW(2328),KW(2329),KW(1138),KW(2330)};
static constexpr std::string_view EKW_698[] = {KW(2331),KW(2332),KW(2333),KW(864),KW(2334),KW(2335),KW(2336),KW(2337)};
static constexpr std::string_view EKW_699[] = {KW(2338),KW(2295),KW(678),KW(2339),KW(2340),KW(2341)};
static constexpr std::string_view EKW_700[] = {KW(2342),KW(2295),KW(678),KW(2343),KW(2344),KW(2341)};
static constexpr std::string_view EKW_701[] = {KW(2345),KW(2295),KW(2317),KW(678),KW(440),KW(441),KW(2346),KW(2155),KW(2347)};
static constexpr std::string_view EKW_702[] = {KW(2348),KW(2317),KW(2295),KW(678),KW(2048)};
static constexpr std::string_view EKW_703[] = {KW(2349),KW(678),KW(2295),KW(2350),KW(1091),KW(1303),KW(2351),KW(2352)};
static constexpr std::string_view EKW_704[] = {KW(2353),KW(2317),KW(2295),KW(348),KW(2354),KW(2326),KW(2327),KW(2350),KW(2355)};
static constexpr std::string_view EKW_705[] = {KW(2356),KW(2317),KW(2295),KW(678),KW(2357),KW(2358),KW(2359)};
static constexpr std::string_view EKW_706[] = {KW(2360),KW(2317),KW(2295),KW(678),KW(936),KW(2357),KW(2361),KW(939)};
static constexpr std::string_view EKW_707[] = {KW(2362),KW(678),KW(2295),KW(2317),KW(2363),KW(2319),KW(1990),KW(2364),KW(2365)};
static constexpr std::string_view EKW_708[] = {KW(2366),KW(678),KW(2295),KW(2317),KW(2367),KW(2368),KW(1988),KW(2319),KW(2365)};
static constexpr std::string_view EKW_709[] = {KW(2369),KW(678),KW(2295),KW(2370),KW(2317),KW(2326),KW(2327),KW(2082),KW(106),KW(2371),KW(364),KW(2367)};
static constexpr std::string_view EKW_710[] = {KW(2372),KW(961),KW(2373)};
static constexpr std::string_view EKW_711[] = {KW(2374),KW(961),KW(2373)};
static constexpr std::string_view EKW_712[] = {KW(2375),KW(2295),KW(2317),KW(2376),KW(2377),KW(2378)};
static constexpr std::string_view EKW_713[] = {KW(2379)};
static constexpr std::string_view EKW_714[] = {KW(2380),KW(2381),KW(1310),KW(2382),KW(2383),KW(2295)};
static constexpr std::string_view EKW_715[] = {KW(2384),KW(2381),KW(678),KW(1310),KW(2385),KW(2386),KW(2387),KW(2295)};
static constexpr std::string_view EKW_716[] = {KW(2388),KW(2381),KW(1310),KW(2389),KW(440),KW(2295)};
static constexpr std::string_view EKW_717[] = {KW(2390),KW(1310),KW(2381),KW(678),KW(2391),KW(2392),KW(2295)};
static constexpr std::string_view EKW_718[] = {KW(2393),KW(2381),KW(678),KW(2394),KW(2395),KW(2295)};
static constexpr std::string_view EKW_719[] = {KW(2396),KW(2381),KW(2397),KW(2398)};
static constexpr std::string_view EKW_720[] = {KW(1939),KW(2381),KW(1310),KW(677),KW(2295),KW(2399)};
static constexpr std::string_view EKW_721[] = {KW(2400),KW(2381),KW(678),KW(1310),KW(2295)};
static constexpr std::string_view EKW_722[] = {KW(2401),KW(2381),KW(1310),KW(2155)};
static constexpr std::string_view EKW_723[] = {KW(2402),KW(2381),KW(2403),KW(1283),KW(133),KW(2295)};
static constexpr std::string_view EKW_724[] = {KW(2404),KW(2381),KW(678),KW(133),KW(2405),KW(2406),KW(2295),KW(2407)};
static constexpr std::string_view EKW_725[] = {KW(2408),KW(2381),KW(678),KW(1310),KW(2295)};
static constexpr std::string_view EKW_726[] = {KW(2409),KW(2381),KW(678),KW(1310),KW(2410),KW(2411),KW(2295)};
static constexpr std::string_view EKW_727[] = {KW(2412),KW(1310),KW(2381),KW(2413),KW(2300),KW(2295),KW(381),KW(157)};
static constexpr std::string_view EKW_728[] = {KW(2414),KW(2381),KW(1310),KW(678),KW(2415),KW(2295)};
static constexpr std::string_view EKW_729[] = {KW(2416),KW(2381),KW(2415),KW(2417),KW(386),KW(2418)};
static constexpr std::string_view EKW_730[] = {KW(2419),KW(2381),KW(1310),KW(1486),KW(2420),KW(2421)};
static constexpr std::string_view EKW_731[] = {KW(2422),KW(2381),KW(2317),KW(678),KW(1310),KW(2295)};
static constexpr std::string_view EKW_732[] = {KW(2423),KW(2381),KW(1310),KW(2424)};
static constexpr std::string_view EKW_733[] = {KW(2347),KW(2381),KW(678),KW(1310),KW(873),KW(2425),KW(2426),KW(2427)};
static constexpr std::string_view EKW_734[] = {KW(2428),KW(2381),KW(1310)};
static constexpr std::string_view EKW_735[] = {KW(2429),KW(2317),KW(678),KW(1310),KW(2430),KW(2431),KW(2295),KW(593)};
static constexpr std::string_view EKW_736[] = {KW(2432),KW(1310),KW(2433),KW(2434),KW(2435),KW(2436),KW(2437),KW(2317)};
static constexpr std::string_view EKW_737[] = {KW(2438),KW(2317),KW(1310),KW(2391)};
static constexpr std::string_view EKW_738[] = {KW(2439),KW(1310),KW(2317),KW(2295),KW(2440),KW(2441),KW(2442)};
static constexpr std::string_view EKW_739[] = {KW(2443),KW(1310),KW(2444),KW(2445),KW(2446),KW(2295)};
static constexpr std::string_view EKW_740[] = {KW(2447),KW(2381),KW(2295),KW(2448),KW(2317)};
static constexpr std::string_view EKW_741[] = {KW(2449),KW(2381),KW(1310),KW(2450),KW(2451),KW(2317)};
static constexpr std::string_view EKW_742[] = {KW(2452),KW(1310),KW(2317),KW(2295),KW(2453),KW(2454),KW(2455),KW(2456),KW(1486),KW(2457),KW(2458),KW(2459)};
static constexpr std::string_view EKW_743[] = {KW(2460),KW(2381),KW(1310),KW(2317),KW(2454),KW(157)};
static constexpr std::string_view EKW_744[] = {KW(2461),KW(1310),KW(2462),KW(1309),KW(2463),KW(2295),KW(2317)};
static constexpr std::string_view EKW_745[] = {KW(2464),KW(1309),KW(1310),KW(2462),KW(2463),KW(2295),KW(2317)};
static constexpr std::string_view EKW_746[] = {KW(2465),KW(1310),KW(2466),KW(2467),KW(2468),KW(2317)};
static constexpr std::string_view EKW_747[] = {KW(2469),KW(1310),KW(2470),KW(2471)};
static constexpr std::string_view EKW_748[] = {KW(2472),KW(1310),KW(2087),KW(2473),KW(2466),KW(2295)};
static constexpr std::string_view EKW_749[] = {KW(2474),KW(2462),KW(1183),KW(1314),KW(2475)};
static constexpr std::string_view EKW_750[] = {KW(2476),KW(2477),KW(364)};
static constexpr std::string_view EKW_751[] = {KW(2478),KW(2378),KW(2376)};
static constexpr std::string_view EKW_752[] = {KW(2479),KW(2480)};
static constexpr std::string_view EKW_753[] = {KW(2481),KW(1310),KW(2352),KW(2482),KW(2483),KW(2484)};
static constexpr std::string_view EKW_754[] = {KW(2485),KW(1310),KW(2481),KW(2486),KW(2482),KW(2487),KW(285)};
static constexpr std::string_view EKW_755[] = {KW(2488),KW(1310),KW(2481),KW(2486),KW(2489),KW(2490)};
static constexpr std::string_view EKW_756[] = {KW(2491),KW(2492),KW(1310),KW(2490),KW(2493),KW(2481),KW(2494),KW(2495),KW(2496),KW(2497)};
static constexpr std::string_view EKW_757[] = {KW(2498),KW(1310),KW(2481),KW(2499),KW(2500),KW(2490),KW(2501),KW(2502)};
static constexpr std::string_view EKW_758[] = {KW(2503),KW(1310),KW(2481),KW(2490),KW(2504),KW(2505),KW(2482),KW(2506),KW(2507)};
static constexpr std::string_view EKW_759[] = {KW(2508),KW(1310),KW(2482),KW(2509),KW(2510),KW(2511),KW(2512),KW(2513),KW(2514),KW(2515)};
static constexpr std::string_view EKW_760[] = {KW(2516),KW(1310),KW(2482),KW(2511),KW(2517),KW(2518)};
static constexpr std::string_view EKW_761[] = {KW(2073),KW(1310),KW(2519),KW(2520)};
static constexpr std::string_view EKW_762[] = {KW(2521),KW(935),KW(1310),KW(2522),KW(2523),KW(2524),KW(2525)};
static constexpr std::string_view EKW_763[] = {KW(2526),KW(1310),KW(2527),KW(2522),KW(961),KW(2120),KW(2118),KW(1117)};
static constexpr std::string_view EKW_764[] = {KW(2528),KW(1310),KW(2019),KW(2527),KW(2529),KW(2530),KW(2531),KW(2532),KW(2533)};
static constexpr std::string_view EKW_765[] = {KW(2534),KW(1310),KW(2482),KW(2535),KW(2030),KW(2527),KW(2511),KW(2536)};
static constexpr std::string_view EKW_766[] = {KW(2537),KW(2527),KW(2538),KW(2017),KW(2539),KW(2540),KW(2541)};
static constexpr std::string_view EKW_767[] = {KW(2542),KW(2543),KW(2544),KW(2538),KW(2432),KW(2545)};
static constexpr std::string_view EKW_768[] = {KW(2546),KW(1310),KW(430),KW(2547),KW(2506),KW(2548),KW(2549)};
static constexpr std::string_view EKW_769[] = {KW(2550),KW(1310),KW(2551),KW(2552),KW(2553),KW(2554),KW(2555),KW(2556)};
static constexpr std::string_view EKW_770[] = {KW(2557),KW(1310),KW(2558),KW(2481),KW(2483),KW(2490),KW(2506),KW(2559),KW(2527),KW(2560)};
static constexpr std::string_view EKW_771[] = {KW(2561),KW(1310),KW(2562)};
static constexpr std::string_view EKW_772[] = {KW(2563),KW(1310),KW(2562),KW(2564)};
static constexpr std::string_view EKW_773[] = {KW(2565),KW(1310),KW(2566),KW(2562),KW(2567),KW(2568)};
static constexpr std::string_view EKW_774[] = {KW(2569),KW(1310),KW(2491),KW(2570),KW(2571),KW(2572),KW(2573),KW(2574),KW(2575),KW(2497),KW(2557),KW(2576)};
static constexpr std::string_view EKW_775[] = {KW(2574),KW(1310),KW(2572),KW(2577),KW(2578),KW(2579)};
static constexpr std::string_view EKW_776[] = {KW(2127),KW(1310),KW(2120),KW(2482),KW(2580)};
static constexpr std::string_view EKW_777[] = {KW(1314),KW(1310),KW(2482),KW(1311),KW(2127),KW(2581),KW(2582),KW(2583),KW(2584)};
static constexpr std::string_view EKW_778[] = {KW(2585),KW(1310),KW(1314),KW(2586),KW(2587),KW(2581),KW(2588)};
static constexpr std::string_view EKW_779[] = {KW(2589),KW(1310),KW(2527),KW(2590),KW(2591),KW(2592),KW(2593)};
static constexpr std::string_view EKW_780[] = {KW(2594),KW(2506),KW(2595),KW(1310),KW(704),KW(2596),KW(2520)};
static constexpr std::string_view EKW_781[] = {KW(2597),KW(1310),KW(2482),KW(2598),KW(2599),KW(2600),KW(2601),KW(2602)};
static constexpr std::string_view EKW_782[] = {KW(2603),KW(1310),KW(2604),KW(2456),KW(2317)};
static constexpr std::string_view EKW_783[] = {KW(2605),KW(1310),KW(2606),KW(2607),KW(2544),KW(2608),KW(2609),KW(2610)};
static constexpr std::string_view EKW_784[] = {KW(2611),KW(1310),KW(1309),KW(2027)};
static constexpr std::string_view EKW_785[] = {KW(2612),KW(2613),KW(2614)};
static constexpr std::string_view EKW_786[] = {KW(2615),KW(1310),KW(2590),KW(2616),KW(2617),KW(2618),KW(2619),KW(2620)};
static constexpr std::string_view EKW_787[] = {KW(2621),KW(1310),KW(1448),KW(703),KW(2558),KW(2622)};
static constexpr std::string_view EKW_788[] = {KW(2623),KW(1310),KW(1448),KW(2544),KW(2624)};
static constexpr std::string_view EKW_789[] = {KW(2625),KW(1310),KW(1448),KW(2626),KW(2627)};
static constexpr std::string_view EKW_790[] = {KW(2628),KW(1310),KW(1487),KW(2629),KW(2592),KW(2630)};
static constexpr std::string_view EKW_791[] = {KW(2631),KW(1310),KW(2444),KW(34),KW(1481)};
static constexpr std::string_view EKW_792[] = {KW(2632),KW(1310),KW(1448),KW(2633),KW(2634),KW(2635),KW(2636),KW(2590)};
static constexpr std::string_view EKW_793[] = {KW(2637),KW(1310),KW(2638),KW(2639),KW(2633)};
static constexpr std::string_view EKW_794[] = {KW(2640),KW(1310),KW(678),KW(2295),KW(2641),KW(2642)};
static constexpr std::string_view EKW_795[] = {KW(2643),KW(2644),KW(1310),KW(1448),KW(2575),KW(2234),KW(1655)};
static constexpr std::string_view EKW_796[] = {KW(2645),KW(1310),KW(2206),KW(1448),KW(2646),KW(2647),KW(2234)};
static constexpr std::string_view EKW_797[] = {KW(2648),KW(1310),KW(660),KW(2649),KW(440),KW(2236),KW(2650)};
static constexpr std::string_view EKW_798[] = {KW(2651),KW(1310),KW(2652),KW(1581),KW(441),KW(2653),KW(713),KW(2654),KW(2655),KW(2656),KW(2635),KW(2657),KW(2658),KW(2659)};
static constexpr std::string_view EKW_799[] = {KW(2660),KW(1310),KW(2368),KW(2661),KW(1532),KW(2662),KW(2663)};
static constexpr std::string_view EKW_800[] = {KW(2664),KW(1310),KW(2661),KW(2665),KW(1448),KW(2523),KW(2527),KW(2666),KW(364),KW(713),KW(2644),KW(1655),KW(119)};
static constexpr std::string_view EKW_801[] = {KW(2667),KW(1310),KW(2668),KW(2669),KW(2670),KW(2671),KW(2672),KW(2673)};
static constexpr std::string_view EKW_802[] = {KW(2674),KW(1310),KW(2675),KW(2661)};
static constexpr std::string_view EKW_803[] = {KW(2676),KW(1310),KW(2677),KW(1486),KW(2678),KW(147),KW(2240),KW(2679),KW(2680)};
static constexpr std::string_view EKW_804[] = {KW(2681),KW(1310),KW(34),KW(2661),KW(440),KW(2682),KW(2683),KW(2684),KW(2665),KW(2685)};
static constexpr std::string_view EKW_805[] = {KW(2686),KW(34),KW(2661),KW(440),KW(2687),KW(1738),KW(2665)};
static constexpr std::string_view EKW_806[] = {KW(2688),KW(1310),KW(34),KW(2661),KW(2592),KW(2665)};
static constexpr std::string_view EKW_807[] = {KW(2689),KW(1310),KW(2661),KW(2544),KW(2665),KW(2690),KW(2482)};
static constexpr std::string_view EKW_808[] = {KW(2691),KW(1310),KW(2544),KW(2692),KW(704),KW(2665),KW(2661),KW(2693),KW(2694)};
static constexpr std::string_view EKW_809[] = {KW(2695),KW(1310),KW(2661),KW(2696),KW(2697),KW(426),KW(430),KW(2659),KW(2665)};
static constexpr std::string_view EKW_810[] = {KW(2698),KW(1310),KW(2661),KW(2696),KW(2659),KW(2699),KW(2549),KW(2414),KW(2665)};
static constexpr std::string_view EKW_811[] = {KW(2700),KW(1310),KW(2661),KW(2490),KW(2665),KW(2696),KW(85),KW(2659)};
static constexpr std::string_view EKW_812[] = {KW(2701),KW(1310),KW(2661),KW(2659),KW(2702),KW(2665)};
static constexpr std::string_view EKW_813[] = {KW(2703),KW(1310),KW(2544),KW(2661),KW(2665),KW(2704)};
static constexpr std::string_view EKW_814[] = {KW(2704),KW(2544),KW(2661),KW(2665),KW(2705)};
static constexpr std::string_view EKW_815[] = {KW(2706),KW(1310),KW(2544),KW(2704),KW(2665),KW(2661),KW(2707),KW(2708)};
static constexpr std::string_view EKW_816[] = {KW(2709),KW(2661),KW(1310),KW(2710),KW(2711),KW(2712),KW(2713),KW(2665)};
static constexpr std::string_view EKW_817[] = {KW(2714),KW(2715),KW(2665),KW(1311),KW(2716)};
static constexpr std::string_view EKW_818[] = {KW(2717),KW(1310),KW(2678),KW(1520),KW(2718),KW(2719)};
static constexpr std::string_view EKW_819[] = {KW(2720),KW(2721),KW(2718),KW(2019)};
static constexpr std::string_view EKW_820[] = {KW(2722),KW(2721),KW(2723),KW(2724),KW(2725),KW(777),KW(2726),KW(2727),KW(704),KW(2718),KW(567),KW(2728)};
static constexpr std::string_view EKW_821[] = {KW(2729),KW(2718),KW(34),KW(2730),KW(2595),KW(2728)};
static constexpr std::string_view EKW_822[] = {KW(2731),KW(2718),KW(2592),KW(2482),KW(364),KW(1437),KW(2721),KW(2732),KW(2733),KW(2728)};
static constexpr std::string_view EKW_823[] = {KW(2734),KW(2382),KW(2718),KW(396),KW(2721),KW(1448),KW(2735),KW(2736),KW(2737),KW(1524),KW(2732),KW(2646)};
static constexpr std::string_view EKW_824[] = {KW(2738),KW(2718),KW(2382),KW(1524),KW(426),KW(2737),KW(2739),KW(2740),KW(430),KW(2741)};
static constexpr std::string_view EKW_825[] = {KW(2742),KW(2718),KW(2721),KW(396),KW(2735),KW(2736),KW(2737),KW(381)};
static constexpr std::string_view EKW_826[] = {KW(2743),KW(2718),KW(396),KW(2735),KW(2721),KW(2736),KW(2346),KW(2737),KW(2744)};
static constexpr std::string_view EKW_827[] = {KW(2745),KW(2721),KW(2746),KW(440),KW(441),KW(2735),KW(2736),KW(2346),KW(2737),KW(2381),KW(990),KW(2747),KW(2748)};
static constexpr std::string_view EKW_828[] = {KW(2749),KW(1609),KW(2721),KW(2718),KW(396),KW(430),KW(2750),KW(440),KW(2735),KW(2736),KW(2737),KW(2751)};
static constexpr std::string_view EKW_829[] = {KW(2752),KW(1609),KW(2721),KW(2718),KW(396),KW(430),KW(2750),KW(440),KW(2735),KW(2736),KW(2737),KW(2753),KW(2754),KW(2755),KW(2756),KW(2726)};
static constexpr std::string_view EKW_830[] = {KW(2757),KW(2721),KW(2718),KW(430),KW(2735),KW(2758),KW(2754),KW(2382),KW(2740),KW(2483),KW(426),KW(2755),KW(2759)};
static constexpr std::string_view EKW_831[] = {KW(2760),KW(2718),KW(2721),KW(396),KW(2735),KW(2761),KW(2736),KW(2762),KW(2763),KW(2764),KW(2759),KW(2765),KW(2766),KW(2767)};
static constexpr std::string_view EKW_832[] = {KW(2768),KW(2732),KW(819),KW(2718),KW(2769),KW(2759),KW(2770)};
static constexpr std::string_view EKW_833[] = {KW(2771),KW(2718),KW(2772),KW(2773),KW(2774),KW(2775),KW(2776),KW(803),KW(2777),KW(2501),KW(2602),KW(819)};
static constexpr std::string_view EKW_834[] = {KW(2778),KW(2779),KW(2780),KW(2781),KW(2782),KW(2783),KW(2242),KW(2784)};
static constexpr std::string_view EKW_835[] = {KW(2785),KW(2718),KW(2774),KW(2782),KW(2665)};
static constexpr std::string_view EKW_836[] = {KW(2786),KW(2718),KW(2728),KW(2721),KW(2787),KW(2788),KW(2789),KW(2790),KW(2791)};
static constexpr std::string_view EKW_837[] = {KW(391),KW(819),KW(37),KW(2792),KW(2793)};
static constexpr std::string_view EKW_838[] = {KW(2634),KW(1310),KW(2794),KW(2795),KW(2796)};
static constexpr std::string_view EKW_839[] = {KW(2797),KW(1310),KW(2798),KW(2799),KW(2558),KW(2800),KW(2801),KW(1314),KW(2802),KW(2803),KW(2602)};
static constexpr std::string_view EKW_840[] = {KW(2804),KW(2802),KW(1311),KW(1314),KW(2805),KW(2602)};
static constexpr std::string_view EKW_841[] = {KW(2806),KW(2802),KW(1311),KW(2602)};
static constexpr std::string_view EKW_842[] = {KW(2807),KW(2808),KW(2809),KW(2802),KW(1311),KW(2810),KW(2811),KW(2530),KW(1314),KW(2529),KW(2812),KW(2813)};
static constexpr std::string_view EKW_843[] = {KW(2814),KW(2678),KW(2815),KW(2613),KW(2769),KW(2816)};
static constexpr std::string_view EKW_844[] = {KW(2817),KW(2818),KW(2814),KW(2819),KW(1314),KW(2718),KW(2820),KW(2813),KW(1152)};
static constexpr std::string_view EKW_845[] = {KW(2821),KW(2822),KW(2823),KW(2824),KW(2825),KW(2826)};
static constexpr std::string_view EKW_846[] = {KW(2827),KW(2822),KW(2823),KW(2828),KW(2824),KW(2825),KW(2826)};
static constexpr std::string_view EKW_847[] = {KW(2829),KW(2822),KW(2823),KW(2830),KW(2824),KW(2825),KW(2826)};
static constexpr std::string_view EKW_848[] = {KW(2831),KW(2824),KW(2825),KW(2823),KW(2832),KW(2833),KW(2834),KW(2835),KW(2836),KW(498),KW(2837),KW(2838),KW(2839)};
static constexpr std::string_view EKW_849[] = {KW(2840),KW(2841),KW(954),KW(2842)};
static constexpr std::string_view EKW_850[] = {KW(2843),KW(2844),KW(2845),KW(1448),KW(2846),KW(1908)};
static constexpr std::string_view EKW_851[] = {KW(2847),KW(2848),KW(2849),KW(2850)};
static constexpr std::string_view EKW_852[] = {KW(2851),KW(2852),KW(678),KW(2853),KW(1737),KW(37)};
static constexpr std::string_view EKW_853[] = {KW(2854),KW(2852),KW(678),KW(2853)};
static constexpr std::string_view EKW_854[] = {KW(2855),KW(2852),KW(678),KW(2856),KW(2857),KW(2854),KW(450)};
static constexpr std::string_view EKW_855[] = {KW(2858),KW(2852),KW(2854),KW(678),KW(1448),KW(2859),KW(2860),KW(1738)};
static constexpr std::string_view EKW_856[] = {KW(2861),KW(2852),KW(2862),KW(2863),KW(2864)};
static constexpr std::string_view EKW_857[] = {KW(2865),KW(450),KW(440),KW(209),KW(2866),KW(2346)};
static constexpr std::string_view EKW_858[] = {KW(2048),KW(2852),KW(2867),KW(2868)};
static constexpr std::string_view EKW_859[] = {KW(2869),KW(2852),KW(2155),KW(2346)};
static constexpr std::string_view EKW_860[] = {KW(2870),KW(2852),KW(2853),KW(678)};
static constexpr std::string_view EKW_861[] = {KW(2871),KW(2852),KW(2872),KW(1723),KW(2873),KW(2874),KW(2875),KW(1719)};
static constexpr std::string_view EKW_862[] = {KW(2876),KW(1387),KW(2877),KW(2878)};
static constexpr std::string_view EKW_863[] = {KW(2879),KW(1460),KW(2880),KW(2881),KW(2882),KW(2883)};
static constexpr std::string_view EKW_864[] = {KW(2884),KW(2885),KW(2886),KW(1461),KW(2887),KW(2888),KW(2889)};
static constexpr std::string_view EKW_865[] = {KW(223),KW(2890),KW(2891),KW(1461),KW(734),KW(2892)};
static constexpr std::string_view EKW_866[] = {KW(2341),KW(678),KW(2893),KW(2894),KW(1461),KW(2895),KW(2896)};
static constexpr std::string_view EKW_867[] = {KW(2897),KW(2333),KW(2898),KW(2899),KW(2900)};
static constexpr std::string_view EKW_868[] = {KW(2901),KW(2902),KW(2852),KW(2903),KW(1913),KW(2333)};
static constexpr std::string_view EKW_869[] = {KW(2904),KW(2905),KW(2906),KW(2907),KW(2903),KW(2908),KW(2909),KW(1176)};
static constexpr std::string_view EKW_870[] = {KW(2333),KW(2903),KW(536)};
static constexpr std::string_view EKW_871[] = {KW(2910),KW(536),KW(2295),KW(678),KW(2903),KW(2370)};
static constexpr std::string_view EKW_872[] = {KW(2911),KW(2903),KW(2912),KW(551),KW(2913),KW(885),KW(2914)};
static constexpr std::string_view EKW_873[] = {KW(2915),KW(2903),KW(695),KW(2916),KW(2652),KW(735),KW(2917)};
static constexpr std::string_view EKW_874[] = {KW(2918),KW(2903),KW(694),KW(2919)};
static constexpr std::string_view EKW_875[] = {KW(1269),KW(2903),KW(1107),KW(2920),KW(347),KW(403),KW(2921),KW(2922),KW(348),KW(381),KW(2923)};
static constexpr std::string_view EKW_876[] = {KW(2924),KW(2903),KW(164),KW(2925),KW(2926),KW(1335),KW(2927),KW(2928),KW(2929),KW(2930)};
static constexpr std::string_view EKW_877[] = {KW(1842),KW(2903),KW(2931),KW(2932),KW(1841),KW(2933)};
static constexpr std::string_view EKW_878[] = {KW(2934),KW(17),KW(90),KW(1853),KW(2903),KW(97),KW(1269)};
static constexpr std::string_view EKW_879[] = {KW(2935),KW(2903),KW(2936),KW(2937),KW(2938),KW(2939),KW(2940),KW(2941),KW(2942),KW(2943),KW(2944)};
static constexpr std::string_view EKW_880[] = {KW(1283),KW(2903),KW(1278),KW(1280),KW(1279),KW(2945),KW(2946),KW(2947),KW(885),KW(890),KW(2948)};
static constexpr std::string_view EKW_881[] = {KW(2949),KW(2903),KW(2936),KW(2950),KW(2951),KW(2952)};
static constexpr std::string_view EKW_882[] = {KW(2953),KW(2903),KW(2954),KW(2955),KW(831),KW(1330),KW(2956)};
static constexpr std::string_view EKW_883[] = {KW(2957),KW(2852),KW(2903),KW(2958)};
static constexpr std::string_view EKW_884[] = {KW(2959),KW(2903),KW(1464),KW(2878),KW(2919),KW(2960)};
static constexpr std::string_view EKW_885[] = {KW(1499),KW(89),KW(17),KW(90),KW(1497),KW(1498),KW(1506),KW(1500),KW(1535),KW(2961),KW(2962),KW(97),KW(698)};
static constexpr std::string_view EKW_886[] = {KW(2963),KW(2852),KW(1448),KW(2964),KW(381)};
static constexpr std::string_view EKW_887[] = {KW(2965),KW(2966),KW(2967),KW(2968),KW(2969)};
static constexpr std::string_view EKW_888[] = {KW(2962),KW(2903),KW(2970),KW(2971),KW(2972),KW(403)};
static constexpr std::string_view EKW_889[] = {KW(2973),KW(2974),KW(1648),KW(2975),KW(2976),KW(1478),KW(2970),KW(2977)};
static constexpr std::string_view EKW_890[] = {KW(2978),KW(2970)};
static constexpr std::string_view EKW_891[] = {KW(2979),KW(2980),KW(1648),KW(2981),KW(2982),KW(2983),KW(2970),KW(2984)};
static constexpr std::string_view EKW_892[] = {KW(2985),KW(2981),KW(2652),KW(2986),KW(2987),KW(2988),KW(242),KW(2970)};
static constexpr std::string_view EKW_893[] = {KW(2989),KW(2990),KW(2973),KW(2974),KW(1478),KW(2970)};
static constexpr std::string_view EKW_894[] = {KW(2991),KW(2852),KW(440),KW(819),KW(2992),KW(2993),KW(2994)};
static constexpr std::string_view EKW_895[] = {KW(2863),KW(2852),KW(2861),KW(2862)};
static constexpr std::string_view EKW_896[] = {KW(2995),KW(2852),KW(2854),KW(2996),KW(2913),KW(258),KW(2997),KW(2998),KW(2999),KW(450)};
static constexpr std::string_view EKW_897[] = {KW(3000),KW(3001),KW(2913),KW(3002),KW(812),KW(100),KW(450)};
static constexpr std::string_view EKW_898[] = {KW(3003),KW(2852),KW(3004),KW(3005),KW(2903),KW(2913),KW(3006)};
static constexpr std::string_view EKW_899[] = {KW(3007),KW(3008),KW(2748),KW(2852),KW(3009),KW(2854),KW(449),KW(450)};
static constexpr std::string_view EKW_900[] = {KW(3010),KW(3009),KW(3008),KW(2748),KW(2852),KW(449),KW(3011),KW(450)};
static constexpr std::string_view EKW_901[] = {KW(3012),KW(2852),KW(3001),KW(3013),KW(2902),KW(2903),KW(2913),KW(3014),KW(2391),KW(449),KW(3011),KW(450)};
static constexpr std::string_view EKW_902[] = {KW(3011),KW(2852),KW(3015),KW(3016),KW(2903),KW(2902),KW(2913),KW(3017),KW(587),KW(449),KW(3010),KW(450)};
static constexpr std::string_view EKW_903[] = {KW(3018),KW(2852),KW(3019),KW(3020),KW(2405),KW(450)};
static constexpr std::string_view EKW_904[] = {KW(3021),KW(3022),KW(2867),KW(1609),KW(3023),KW(3024),KW(254),KW(567)};
static constexpr std::string_view EKW_905[] = {KW(3025),KW(2852),KW(3026),KW(1535),KW(3027),KW(3028),KW(2773),KW(3029),KW(1014)};
static constexpr std::string_view EKW_906[] = {KW(3030),KW(1686),KW(2994),KW(3031),KW(649)};
static constexpr std::string_view EKW_907[] = {KW(3032),KW(2852),KW(3026),KW(3033),KW(1535),KW(3031),KW(28),KW(3027),KW(3028),KW(3034),KW(2994)};
static constexpr std::string_view EKW_908[] = {KW(3035),KW(3026),KW(3036),KW(2852),KW(1686),KW(1535),KW(3031),KW(3027),KW(2994),KW(3037),KW(3038)};
static constexpr std::string_view EKW_909[] = {KW(3039),KW(1180),KW(1611),KW(3040),KW(3041),KW(3042),KW(1618),KW(2944),KW(2003)};
static constexpr std::string_view EKW_910[] = {KW(3043),KW(1532),KW(3026),KW(430),KW(1535),KW(28),KW(3027),KW(3044)};
static constexpr std::string_view EKW_911[] = {KW(3045),KW(1794),KW(1758),KW(3046),KW(3047),KW(3048),KW(254)};
static constexpr std::string_view EKW_912[] = {KW(3049),KW(1794),KW(1758),KW(3050),KW(3051),KW(3052),KW(3053),KW(3046),KW(3054),KW(3055),KW(3056)};
static constexpr std::string_view EKW_913[] = {KW(3057),KW(1794),KW(1758),KW(3058),KW(3048),KW(3059),KW(329)};
static constexpr std::string_view EKW_914[] = {KW(3060),KW(1794),KW(1758),KW(2000),KW(828),KW(3061),KW(2842),KW(3058),KW(3062),KW(885),KW(304),KW(3048),KW(3059)};
static constexpr std::string_view EKW_915[] = {KW(3046),KW(1794),KW(1758),KW(3063),KW(3051),KW(3064),KW(3048),KW(3065),KW(3066)};
static constexpr std::string_view EKW_916[] = {KW(3067),KW(1794),KW(3068),KW(3069),KW(3070),KW(3071),KW(3072),KW(3046),KW(1758)};
static constexpr std::string_view EKW_917[] = {KW(3073),KW(1794),KW(1758),KW(3048)};
static constexpr std::string_view EKW_918[] = {KW(3074),KW(1794),KW(1758),KW(3061),KW(3075),KW(3048),KW(3046)};
static constexpr std::string_view EKW_919[] = {KW(3054),KW(1794),KW(1758),KW(3055)};
static constexpr std::string_view EKW_920[] = {KW(3076),KW(1794),KW(1758)};
static constexpr std::string_view EKW_921[] = {KW(3077),KW(1794),KW(1758),KW(3078),KW(3079),KW(3046)};
static constexpr std::string_view EKW_922[] = {KW(3080),KW(1794),KW(1758),KW(3050),KW(3061),KW(2842),KW(3046),KW(3055)};
static constexpr std::string_view EKW_923[] = {KW(3081),KW(3078),KW(1758),KW(1794),KW(1283)};
static constexpr std::string_view EKW_924[] = {KW(3082),KW(1758),KW(1794),KW(3062)};
static constexpr std::string_view EKW_925[] = {KW(3055),KW(3083),KW(1794),KW(1758),KW(3081),KW(3084),KW(3054),KW(3085)};
static constexpr std::string_view EKW_926[] = {KW(3086),KW(1758),KW(3078),KW(1794),KW(3081),KW(3087),KW(3088),KW(1710)};
static constexpr std::string_view EKW_927[] = {KW(3089),KW(1107),KW(1425),KW(1269),KW(1758)};
static constexpr std::string_view EKW_928[] = {KW(3090),KW(1794),KW(3091),KW(1758),KW(3092),KW(3093)};
static constexpr std::string_view EKW_929[] = {KW(3094),KW(1758),KW(3091),KW(1794),KW(1294),KW(1422),KW(1423),KW(1417),KW(3095),KW(329)};
static constexpr std::string_view EKW_930[] = {KW(3096),KW(1758),KW(1294),KW(1422),KW(1423),KW(1425),KW(3097),KW(3098)};
static constexpr std::string_view EKW_931[] = {KW(3099),KW(3100),KW(1758),KW(3091),KW(1794),KW(2968),KW(329),KW(3101),KW(2969)};
static constexpr std::string_view EKW_932[] = {KW(3102),KW(1758),KW(3091),KW(3100),KW(3062),KW(3101)};
static constexpr std::string_view EKW_933[] = {KW(3103),KW(381),KW(1794),KW(1758),KW(3078),KW(329)};
static constexpr std::string_view EKW_934[] = {KW(3104),KW(3078),KW(1758),KW(1794),KW(3062)};
static constexpr std::string_view EKW_935[] = {KW(3105),KW(1794),KW(1758),KW(386),KW(3106),KW(3078),KW(3107),KW(3108),KW(3109)};
static constexpr std::string_view EKW_936[] = {KW(3110),KW(3078),KW(1794),KW(1758)};
static constexpr std::string_view EKW_937[] = {KW(3111),KW(3091),KW(1794),KW(1758),KW(3112)};
static constexpr std::string_view EKW_938[] = {KW(3113),KW(1758),KW(3091),KW(1794),KW(3114),KW(364),KW(3115),KW(3093)};
static constexpr std::string_view EKW_939[] = {KW(3116),KW(1758),KW(3078),KW(1303),KW(3117),KW(1302)};
static constexpr std::string_view EKW_940[] = {KW(3118),KW(1723),KW(1678),KW(828),KW(3119),KW(3120),KW(154)};
static constexpr std::string_view EKW_941[] = {KW(3121),KW(1678),KW(1723),KW(828),KW(3122),KW(1679)};
static constexpr std::string_view EKW_942[] = {KW(3123),KW(1758),KW(3124),KW(3125),KW(1785),KW(3126)};
static constexpr std::string_view EKW_943[] = {KW(3127),KW(1083)};
static constexpr std::string_view EKW_944[] = {KW(3128),KW(1083)};
static constexpr std::string_view EKW_945[] = {KW(3129),KW(1623),KW(1794),KW(3130)};
static constexpr std::string_view EKW_946[] = {KW(1784),KW(1785),KW(1723),KW(38),KW(1789),KW(3131),KW(1758)};
static constexpr std::string_view EKW_947[] = {KW(3132),KW(1758),KW(1087),KW(3133)};
static constexpr std::string_view EKW_948[] = {KW(3134),KW(3135),KW(3136)};
static constexpr std::string_view EKW_949[] = {KW(3137),KW(3138),KW(1723),KW(3139),KW(3140)};
static constexpr std::string_view EKW_950[] = {KW(3141),KW(1794),KW(3142),KW(3143)};
static constexpr std::string_view EKW_951[] = {KW(3144),KW(3145),KW(3146),KW(3147),KW(3148)};
static constexpr std::string_view EKW_952[] = {KW(3149),KW(3046),KW(1794)};
static constexpr std::string_view EKW_953[] = {KW(3150),KW(3151)};
static constexpr std::string_view EKW_954[] = {KW(3152),KW(3153),KW(3154),KW(3063),KW(3155),KW(3156)};
static constexpr std::string_view EKW_955[] = {KW(3157),KW(3078),KW(3158),KW(815),KW(3159),KW(3160)};
static constexpr std::string_view EKW_956[] = {KW(3161),KW(1421),KW(3089),KW(1425),KW(2921),KW(3162),KW(3163),KW(3164),KW(1294),KW(1422),KW(3165),KW(3091),KW(3166),KW(3167),KW(3168),KW(3169),KW(3170),KW(3171),KW(1758),KW(3172)};
static constexpr std::string_view EKW_957[] = {KW(3173),KW(1794),KW(3174)};
static constexpr std::string_view EKW_958[] = {KW(3175),KW(1794),KW(3176),KW(3177),KW(3174)};
static constexpr std::string_view EKW_959[] = {KW(3178),KW(882),KW(3179)};
static constexpr std::string_view EKW_960[] = {KW(1461),KW(1460),KW(2881),KW(3180),KW(3172),KW(3181),KW(761),KW(3182),KW(165),KW(3183),KW(1183),KW(3184)};
static constexpr std::string_view EKW_961[] = {KW(3185),KW(1760),KW(3186),KW(1581),KW(3187),KW(3188),KW(3189),KW(3190),KW(2813)};
static constexpr std::string_view EKW_962[] = {KW(3191),KW(3192),KW(3193),KW(3194),KW(3195),KW(1456)};
static constexpr std::string_view EKW_963[] = {KW(3196),KW(1760),KW(440),KW(1794),KW(819),KW(3197),KW(3187),KW(3198),KW(3199),KW(1581),KW(1758),KW(3200)};
static constexpr std::string_view EKW_964[] = {KW(3201),KW(3187),KW(3202),KW(819),KW(1760)};
static constexpr std::string_view EKW_965[] = {KW(3203),KW(1760),KW(1794),KW(1758),KW(440),KW(3187),KW(3204),KW(3205)};
static constexpr std::string_view EKW_966[] = {KW(3206),KW(3200),KW(3207),KW(3186),KW(1758)};
static constexpr std::string_view EKW_967[] = {KW(3186),KW(3187),KW(1760),KW(3200),KW(3064)};
static constexpr std::string_view EKW_968[] = {KW(3208),KW(1760),KW(3204),KW(1758)};
static constexpr std::string_view EKW_969[] = {KW(1760),KW(1794),KW(3209),KW(3210),KW(3187),KW(3207),KW(3064),KW(1758)};
static constexpr std::string_view EKW_970[] = {KW(1396),KW(1758),KW(1794),KW(2150),KW(1394),KW(3211),KW(1395)};
static constexpr std::string_view EKW_971[] = {KW(3212),KW(2150),KW(1794),KW(1394),KW(1758),KW(3211),KW(1395)};
static constexpr std::string_view EKW_972[] = {KW(3213),KW(3214),KW(2150),KW(3215),KW(3211),KW(3216),KW(3217),KW(1395),KW(3218),KW(1758)};
static constexpr std::string_view EKW_973[] = {KW(3219),KW(3214),KW(2150),KW(3220),KW(3211),KW(3221),KW(3222),KW(3215),KW(1395),KW(1758)};
static constexpr std::string_view EKW_974[] = {KW(3223),KW(1394),KW(3224),KW(3225),KW(3226),KW(3227)};
static constexpr std::string_view EKW_975[] = {KW(3228),KW(3229),KW(1396),KW(3158),KW(3081),KW(2150),KW(1394),KW(3211),KW(3230),KW(3046)};
static constexpr std::string_view EKW_976[] = {KW(3231),KW(1794),KW(1758),KW(1394)};
static constexpr std::string_view EKW_977[] = {KW(3232),KW(1758),KW(1794)};
static constexpr std::string_view EKW_978[] = {KW(3233),KW(1794),KW(1758),KW(1739),KW(3234),KW(3235)};
static constexpr std::string_view EKW_979[] = {KW(3236),KW(1794),KW(1758),KW(1739),KW(3234),KW(3078),KW(3235),KW(3237)};
static constexpr std::string_view EKW_980[] = {KW(3238),KW(2916),KW(3239),KW(3240),KW(3241),KW(3242),KW(3243),KW(3244),KW(652),KW(1758)};
static constexpr std::string_view EKW_981[] = {KW(1405),KW(3245),KW(1760),KW(3246),KW(3242),KW(3247),KW(642),KW(1394),KW(3248),KW(1758),KW(3210)};
static constexpr std::string_view EKW_982[] = {KW(3249),KW(1794),KW(1758),KW(653),KW(639),KW(644),KW(86),KW(652)};
static constexpr std::string_view EKW_983[] = {KW(3250),KW(1315),KW(1842)};
static constexpr std::string_view EKW_984[] = {KW(3251),KW(3252),KW(2842),KW(3253)};
static constexpr std::string_view EKW_985[] = {KW(3254),KW(3255),KW(2946),KW(3256),KW(3257),KW(793),KW(794),KW(795),KW(2866),KW(3258)};
static constexpr std::string_view EKW_986[] = {KW(3259),KW(3256),KW(3255),KW(3260),KW(3261),KW(2866),KW(3258)};
static constexpr std::string_view EKW_987[] = {KW(1118),KW(3255),KW(3262),KW(3263),KW(2946),KW(3264),KW(3265),KW(3266)};
static constexpr std::string_view EKW_988[] = {KW(3267),KW(3255),KW(3268),KW(3009)};
static constexpr std::string_view EKW_989[] = {KW(3269),KW(3255),KW(3270),KW(2946)};
static constexpr std::string_view EKW_990[] = {KW(3271),KW(719)};
static constexpr std::string_view EKW_991[] = {KW(3272),KW(3255)};
static constexpr std::string_view EKW_992[] = {KW(3273),KW(3274),KW(3275),KW(3276),KW(3277),KW(3278),KW(3255),KW(3279),KW(3280),KW(3281),KW(3282),KW(3283),KW(3284),KW(3285),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_993[] = {KW(3288),KW(3289),KW(3290),KW(3291),KW(3292),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3293),KW(1)};
static constexpr std::string_view EKW_994[] = {KW(3294),KW(3295),KW(3296),KW(798),KW(3297),KW(3298),KW(3255),KW(3282),KW(3283),KW(3284),KW(3299),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_995[] = {KW(3300),KW(3301),KW(3302),KW(3303),KW(3304),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3305),KW(1)};
static constexpr std::string_view EKW_996[] = {KW(3306),KW(3307),KW(3308),KW(3309),KW(3310),KW(3311),KW(3255),KW(3282),KW(3283),KW(3284),KW(3312),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_997[] = {KW(3313),KW(3314),KW(3315),KW(3316),KW(3317),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3318),KW(1)};
static constexpr std::string_view EKW_998[] = {KW(3319),KW(110),KW(3320),KW(3321),KW(3322),KW(3323),KW(3255),KW(3282),KW(3283),KW(3284),KW(3324),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_999[] = {KW(3325),KW(3326),KW(3327),KW(3328),KW(3329),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3330),KW(1)};
static constexpr std::string_view EKW_1000[] = {KW(3331),KW(3332),KW(3333),KW(3334),KW(3335),KW(3336),KW(3255),KW(3282),KW(3283),KW(3284),KW(3337),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1001[] = {KW(3338),KW(3339),KW(3340),KW(3341),KW(3342),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3343),KW(1)};
static constexpr std::string_view EKW_1002[] = {KW(3344),KW(3345),KW(3346),KW(3347),KW(3348),KW(3349),KW(3255),KW(3282),KW(3283),KW(3284),KW(3350),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1003[] = {KW(3351),KW(3352),KW(3353),KW(3354),KW(3355),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3356),KW(1)};
static constexpr std::string_view EKW_1004[] = {KW(3357),KW(3358),KW(3359),KW(3360),KW(3361),KW(3362),KW(3255),KW(3282),KW(3283),KW(3284),KW(3016),KW(3017),KW(3363),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1005[] = {KW(3364),KW(3365),KW(3366),KW(3367),KW(3368),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3369),KW(1)};
static constexpr std::string_view EKW_1006[] = {KW(3370),KW(3371),KW(3372),KW(3373),KW(3374),KW(3375),KW(3255),KW(3282),KW(3283),KW(3284),KW(3376),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1007[] = {KW(3377),KW(3378),KW(3379),KW(3380),KW(3381),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3382),KW(1)};
static constexpr std::string_view EKW_1008[] = {KW(3383),KW(3384),KW(3385),KW(3386),KW(3387),KW(3388),KW(3255),KW(3282),KW(3283),KW(3284),KW(3389),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1009[] = {KW(3390),KW(3391),KW(3392),KW(3393),KW(3394),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3395),KW(1)};
static constexpr std::string_view EKW_1010[] = {KW(3396),KW(3397),KW(3398),KW(3399),KW(3400),KW(3401),KW(3255),KW(3282),KW(3283),KW(3284),KW(3402),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1011[] = {KW(3403),KW(3404),KW(3405),KW(3406),KW(3407),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3408),KW(1)};
static constexpr std::string_view EKW_1012[] = {KW(3409),KW(3410),KW(3411),KW(3412),KW(3413),KW(3414),KW(3255),KW(3282),KW(3283),KW(3284),KW(3415),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1013[] = {KW(3416),KW(3417),KW(3418),KW(3419),KW(3420),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3421),KW(1)};
static constexpr std::string_view EKW_1014[] = {KW(3422),KW(3423),KW(3424),KW(3425),KW(3426),KW(3427),KW(3255),KW(3282),KW(3283),KW(3284),KW(3428),KW(1),KW(3286),KW(3287)};
static constexpr std::string_view EKW_1015[] = {KW(3429),KW(3430),KW(3431),KW(3432),KW(3433),KW(3255),KW(3282),KW(3283),KW(3284),KW(2946),KW(3434),KW(1)};
static constexpr std::string_view EKW_1016[] = {KW(3435),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(779),KW(3436),KW(3437),KW(3438),KW(801),KW(450)};
static constexpr std::string_view EKW_1017[] = {KW(3439),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(801),KW(450)};
static constexpr std::string_view EKW_1018[] = {KW(3440),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(801),KW(450)};
static constexpr std::string_view EKW_1019[] = {KW(3441),KW(678),KW(336),KW(3013),KW(3442),KW(1574),KW(2826),KW(652),KW(3001),KW(330),KW(801),KW(450)};
static constexpr std::string_view EKW_1020[] = {KW(3443),KW(678),KW(1183),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(801),KW(450)};
static constexpr std::string_view EKW_1021[] = {KW(3444),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(3441),KW(801),KW(450)};
static constexpr std::string_view EKW_1022[] = {KW(3445),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(801),KW(450)};
static constexpr std::string_view EKW_1023[] = {KW(3446),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(801),KW(450)};
static constexpr std::string_view EKW_1024[] = {KW(3447),KW(336),KW(330),KW(3013),KW(3001),KW(814),KW(652),KW(450)};
static constexpr std::string_view EKW_1025[] = {KW(3448),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(600),KW(779),KW(3449),KW(450)};
static constexpr std::string_view EKW_1026[] = {KW(3450),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(450)};
static constexpr std::string_view EKW_1027[] = {KW(3451),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(450)};
static constexpr std::string_view EKW_1028[] = {KW(352),KW(450),KW(351),KW(34),KW(37)};
static constexpr std::string_view EKW_1029[] = {KW(449),KW(450),KW(678),KW(3452),KW(440),KW(441),KW(2082),KW(761),KW(444),KW(3453),KW(652),KW(209),KW(3454)};
static constexpr std::string_view EKW_1030[] = {KW(3455),KW(678),KW(1574),KW(2826),KW(652),KW(336),KW(3001),KW(330),KW(444),KW(3456),KW(7),KW(14),KW(450)};
static constexpr std::string_view EKW_1031[] = {KW(3457),KW(678),KW(3009),KW(3013),KW(444),KW(7),KW(14),KW(652),KW(440),KW(3458),KW(450)};
static constexpr std::string_view EKW_1032[] = {KW(3459),KW(1402),KW(1404),KW(3460),KW(3461),KW(652)};
static constexpr std::string_view EKW_1033[] = {KW(812),KW(336),KW(1183),KW(766),KW(3462),KW(119)};
static constexpr std::string_view EKW_1034[] = {KW(3463),KW(336),KW(709),KW(975),KW(935),KW(814),KW(3464),KW(3465),KW(3466),KW(3467)};
static constexpr std::string_view EKW_1035[] = {KW(3468),KW(336),KW(2852),KW(1535),KW(2365),KW(3469),KW(652),KW(1406),KW(3470),KW(3471),KW(1045),KW(971)};
static constexpr std::string_view EKW_1036[] = {KW(3472),KW(2852),KW(652),KW(1406),KW(3473),KW(336),KW(3013),KW(3474),KW(450)};
static constexpr std::string_view EKW_1037[] = {KW(862),KW(450),KW(3013),KW(3475),KW(3476)};
static constexpr std::string_view EKW_1038[] = {KW(3477),KW(450),KW(678),KW(3475),KW(3009),KW(2319),KW(2082),KW(3478),KW(209)};
static constexpr std::string_view EKW_1039[] = {KW(3479),KW(450),KW(3480),KW(3481)};
static constexpr std::string_view EKW_1040[] = {KW(3482),KW(450),KW(119)};
static constexpr std::string_view EKW_1041[] = {KW(3483),KW(450),KW(119)};
static constexpr std::string_view EKW_1042[] = {KW(3484),KW(450),KW(119)};
static constexpr std::string_view EKW_1043[] = {KW(3485),KW(450)};
static constexpr std::string_view EKW_1044[] = {KW(3486),KW(450),KW(37)};
static constexpr std::string_view EKW_1045[] = {KW(3487),KW(450),KW(3481)};
static constexpr std::string_view EKW_1046[] = {KW(3488),KW(450),KW(3489),KW(3490),KW(862),KW(3491)};
static constexpr std::string_view EKW_1047[] = {KW(258),KW(450),KW(862)};
static constexpr std::string_view EKW_1048[] = {KW(3492),KW(834),KW(827),KW(106),KW(419),KW(862),KW(1521),KW(678),KW(450)};
static constexpr std::string_view EKW_1049[] = {KW(3489),KW(450),KW(2654),KW(386),KW(862),KW(3493),KW(3494),KW(3495),KW(3496),KW(3488),KW(3497),KW(3498),KW(306),KW(3490)};
static constexpr std::string_view EKW_1050[] = {KW(3499),KW(678),KW(3),KW(3500),KW(2852),KW(3013),KW(2082),KW(1857),KW(1851),KW(562),KW(3501),KW(3502),KW(450)};
static constexpr std::string_view EKW_1051[] = {KW(3503),KW(450),KW(3502),KW(3504),KW(1931),KW(3505),KW(713)};
static constexpr std::string_view EKW_1052[] = {KW(3506),KW(450),KW(2082),KW(1931),KW(13),KW(3502)};
static constexpr std::string_view EKW_1053[] = {KW(3507),KW(3508),KW(450),KW(2082),KW(1931),KW(902),KW(3509)};
static constexpr std::string_view EKW_1054[] = {KW(3510),KW(450),KW(440),KW(441),KW(3511),KW(3502),KW(449)};
static constexpr std::string_view EKW_1055[] = {KW(3512),KW(3481),KW(450),KW(3513),KW(828),KW(3514),KW(603),KW(3051),KW(3515),KW(165),KW(3516),KW(2000)};
static constexpr std::string_view EKW_1056[] = {KW(3517),KW(1737),KW(3518),KW(37),KW(450),KW(1544),KW(1533),KW(1738),KW(3519)};
static constexpr std::string_view EKW_1057[] = {KW(3520),KW(1737),KW(3518),KW(37),KW(450),KW(1544),KW(1533),KW(388),KW(1738),KW(3521),KW(3519)};
static constexpr std::string_view EKW_1058[] = {KW(3522),KW(1737),KW(3518),KW(37),KW(450),KW(1544),KW(1533),KW(388),KW(3523),KW(3524),KW(3525)};
static constexpr std::string_view EKW_1059[] = {KW(3526),KW(652)};
static constexpr std::string_view EKW_1060[] = {KW(1411),KW(34),KW(1309),KW(3527),KW(747),KW(3528),KW(3529),KW(2813),KW(3530)};
static constexpr std::string_view EKW_1061[] = {KW(3531),KW(819),KW(820),KW(3532),KW(2082),KW(37),KW(618),KW(902),KW(35),KW(450)};
static constexpr std::string_view EKW_1062[] = {KW(3533),KW(1581),KW(819),KW(867),KW(678),KW(3534),KW(2856),KW(441),KW(1753),KW(3535),KW(450)};
static constexpr std::string_view EKW_1063[] = {KW(3536),KW(605),KW(2151),KW(3537),KW(600),KW(2319),KW(1535),KW(426),KW(3027),KW(3538)};
static constexpr std::string_view EKW_1064[] = {KW(3539),KW(1532),KW(2748),KW(3540),KW(1533),KW(426),KW(1535),KW(3027),KW(3541)};
static constexpr std::string_view EKW_1065[] = {KW(3542),KW(2852),KW(1532),KW(3026),KW(3543),KW(1535),KW(426),KW(3027),KW(420)};
static constexpr std::string_view EKW_1066[] = {KW(3544),KW(1406),KW(336),KW(3545),KW(1535),KW(426),KW(3027),KW(3546),KW(3542),KW(3547),KW(3548),KW(709)};
static constexpr std::string_view EKW_1067[] = {KW(3549),KW(3550),KW(809),KW(807),KW(420),KW(3551),KW(3542)};
static constexpr std::string_view EKW_1068[] = {KW(3552),KW(1406),KW(3545),KW(3553),KW(439),KW(975),KW(935),KW(814),KW(3027),KW(3554),KW(709),KW(812)};
static constexpr std::string_view EKW_1069[] = {KW(858),KW(430),KW(426),KW(428),KW(1938),KW(1535),KW(3027),KW(381)};
static constexpr std::string_view EKW_1070[] = {KW(3555),KW(430),KW(3543),KW(428),KW(814),KW(1938),KW(426),KW(3556),KW(1535),KW(3027),KW(424),KW(1016)};
static constexpr std::string_view EKW_1071[] = {KW(3557),KW(1532),KW(430),KW(428),KW(1938),KW(1535),KW(426),KW(3027)};
static constexpr std::string_view EKW_1072[] = {KW(3558),KW(2295),KW(678),KW(2930),KW(440),KW(3559),KW(1045),KW(3560),KW(3561),KW(1535),KW(3562),KW(426),KW(3027),KW(1448)};
static constexpr std::string_view EKW_1073[] = {KW(3563),KW(1448),KW(2295),KW(678),KW(2317),KW(2100),KW(3564),KW(3559),KW(1535),KW(426),KW(3565),KW(3566)};
static constexpr std::string_view EKW_1074[] = {KW(3567),KW(1448),KW(1959),KW(3568),KW(1535),KW(426),KW(3569),KW(3027),KW(1532),KW(3570),KW(3571)};
static constexpr std::string_view EKW_1075[] = {KW(3572),KW(2206),KW(1448),KW(3573),KW(3574),KW(3562),KW(1535),KW(426),KW(3027),KW(3575),KW(3576),KW(3577),KW(826)};
static constexpr std::string_view EKW_1076[] = {KW(3578),KW(678),KW(3579),KW(2082),KW(3580),KW(1535),KW(426),KW(3027),KW(3581),KW(2225)};
static constexpr std::string_view EKW_1077[] = {KW(3582),KW(2852),KW(2652),KW(2846),KW(3583),KW(1535),KW(2368),KW(426),KW(3584),KW(3027),KW(1532),KW(2326),KW(3585),KW(3586),KW(2646),KW(3587)};
static constexpr std::string_view EKW_1078[] = {KW(3588),KW(706),KW(3589),KW(935),KW(3590),KW(3591),KW(939),KW(164),KW(3592),KW(3593),KW(1121)};
static constexpr std::string_view EKW_1079[] = {KW(3594),KW(729),KW(713),KW(1135),KW(3595),KW(1052),KW(426)};
static constexpr std::string_view EKW_1080[] = {KW(3596),KW(3597),KW(428),KW(1544),KW(1533),KW(703),KW(426),KW(3027)};
static constexpr std::string_view EKW_1081[] = {KW(3598),KW(1723),KW(3599),KW(1027),KW(3600),KW(426)};
static constexpr std::string_view EKW_1082[] = {KW(3601),KW(1723),KW(2873),KW(3602),KW(3027),KW(3603)};
static constexpr std::string_view EKW_1083[] = {KW(3603),KW(3604),KW(2873),KW(796),KW(1535),KW(3605),KW(3027),KW(3606),KW(3607),KW(2823)};
static constexpr std::string_view EKW_1084[] = {KW(3608),KW(3609),KW(3610),KW(3611),KW(426),KW(729),KW(3612)};
static constexpr std::string_view EKW_1085[] = {KW(3613),KW(3614),KW(3609),KW(3615),KW(2872),KW(3616),KW(3617),KW(3618),KW(3619),KW(3620),KW(3621)};
static constexpr std::string_view EKW_1086[] = {KW(3622),KW(3609),KW(3610),KW(766),KW(3620)};
static constexpr std::string_view EKW_1087[] = {KW(3623),KW(3609),KW(3610),KW(3624),KW(766)};
static constexpr std::string_view EKW_1088[] = {KW(3625),KW(3609),KW(3626),KW(782)};
static constexpr std::string_view EKW_1089[] = {KW(3627),KW(3609),KW(3628),KW(3629)};
static constexpr std::string_view EKW_1090[] = {KW(3630),KW(1723),KW(3631)};
static constexpr std::string_view EKW_1091[] = {KW(3632),KW(1723),KW(2666),KW(1744),KW(3633)};
static constexpr std::string_view EKW_1092[] = {KW(3633),KW(1723),KW(2666),KW(1744),KW(647),KW(3634),KW(1719),KW(3635)};
static constexpr std::string_view EKW_1093[] = {KW(1770),KW(1723),KW(2666),KW(3636),KW(1744),KW(3637),KW(2391)};
static constexpr std::string_view EKW_1094[] = {KW(3638),KW(1723),KW(2666),KW(1744),KW(647)};
static constexpr std::string_view EKW_1095[] = {KW(3639),KW(1723),KW(2666),KW(3640),KW(1744),KW(3641),KW(3642)};
static constexpr std::string_view EKW_1096[] = {KW(3643),KW(1723),KW(1914),KW(1744),KW(3644),KW(3645)};
static constexpr std::string_view EKW_1097[] = {KW(3646),KW(1723),KW(2666),KW(364),KW(1744),KW(3647),KW(3648)};
static constexpr std::string_view EKW_1098[] = {KW(3649),KW(1723),KW(3650),KW(3651),KW(647),KW(1013),KW(1719)};
static constexpr std::string_view EKW_1099[] = {KW(3652),KW(1723),KW(1686),KW(649),KW(1744),KW(647),KW(3653),KW(3654),KW(3655),KW(3656)};
static constexpr std::string_view EKW_1100[] = {KW(3657),KW(1723),KW(1744),KW(2092),KW(3658)};
static constexpr std::string_view EKW_1101[] = {KW(3659),KW(1723),KW(1744),KW(647),KW(1655)};
static constexpr std::string_view EKW_1102[] = {KW(3660),KW(1723),KW(647),KW(1568),KW(1655)};
static constexpr std::string_view EKW_1103[] = {KW(3661),KW(1723),KW(1744),KW(1655),KW(647),KW(3662),KW(1719)};
static constexpr std::string_view EKW_1104[] = {KW(3663),KW(1723),KW(3664),KW(1744),KW(2092),KW(647),KW(3202),KW(3665),KW(3646)};
static constexpr std::string_view EKW_1105[] = {KW(3666),KW(1723),KW(3667),KW(647),KW(3648),KW(3668)};
static constexpr std::string_view EKW_1106[] = {KW(3669),KW(1723),KW(3670)};
static constexpr std::string_view EKW_1107[] = {KW(3671),KW(3672),KW(3673),KW(3674)};
static constexpr std::string_view EKW_1108[] = {KW(3675),KW(1723),KW(908)};
static constexpr std::string_view EKW_1109[] = {KW(3676),KW(1723),KW(1335),KW(3575),KW(837),KW(440),KW(1013)};
static constexpr std::string_view EKW_1110[] = {KW(3677),KW(1723),KW(3678)};
static constexpr std::string_view EKW_1111[] = {KW(3679),KW(1310),KW(1724),KW(440),KW(3027),KW(2206),KW(3680)};
static constexpr std::string_view EKW_1112[] = {KW(3681),KW(1719),KW(1753),KW(3682),KW(3683)};
static constexpr std::string_view EKW_1113[] = {KW(3684),KW(649),KW(3685),KW(3686),KW(1674),KW(3687),KW(3688)};
static constexpr std::string_view EKW_1114[] = {KW(3689),KW(1723),KW(1737),KW(37),KW(1738),KW(3690),KW(1739),KW(3691)};
static constexpr std::string_view EKW_1115[] = {KW(3692),KW(3693),KW(3694),KW(3695),KW(3696)};
static constexpr std::string_view EKW_1116[] = {KW(3697),KW(1723),KW(647),KW(223)};
static constexpr std::string_view EKW_1117[] = {KW(3698),KW(647),KW(649),KW(2737),KW(3699),KW(3700),KW(1535),KW(3701),KW(2020),KW(3702),KW(3703),KW(3027),KW(18)};
static constexpr std::string_view EKW_1118[] = {KW(3704),KW(1959),KW(3705),KW(3706)};
static constexpr std::string_view EKW_1119[] = {KW(3707),KW(826),KW(1394),KW(3708),KW(1959)};
static constexpr std::string_view EKW_1120[] = {KW(3709),KW(992),KW(2810),KW(3710),KW(3711),KW(3712),KW(813),KW(3713),KW(2813),KW(819)};
static constexpr std::string_view EKW_1121[] = {KW(3714),KW(1763),KW(1724),KW(647),KW(939),KW(814),KW(3715),KW(3716),KW(3717),KW(3718),KW(3719),KW(3720)};
static constexpr std::string_view EKW_1122[] = {KW(3721),KW(1692),KW(430),KW(814),KW(1938),KW(3722),KW(3723),KW(85),KW(86),KW(3724),KW(593),KW(87),KW(2813)};
static constexpr std::string_view EKW_1123[] = {KW(3725),KW(3726),KW(988),KW(1562),KW(1563)};
static constexpr std::string_view EKW_1124[] = {KW(3727),KW(649),KW(3728),KW(3729),KW(3730),KW(3027),KW(3731),KW(3732),KW(3733),KW(3734),KW(3735)};
static constexpr std::string_view EKW_1125[] = {KW(3736),KW(647),KW(649),KW(3027),KW(654)};
static constexpr std::string_view EKW_1126[] = {KW(3737),KW(3738),KW(1997),KW(3739),KW(3740),KW(939),KW(3741),KW(1535),KW(1733),KW(647),KW(3742)};
static constexpr std::string_view EKW_1127[] = {KW(3743),KW(3744),KW(3745),KW(3746),KW(649),KW(939),KW(3027),KW(1733)};
static constexpr std::string_view EKW_1128[] = {KW(3747),KW(3748),KW(3749),KW(2699),KW(3750),KW(3751)};
static constexpr std::string_view EKW_1129[] = {KW(3752),KW(3753),KW(2570),KW(3754),KW(1959)};
static constexpr std::string_view EKW_1130[] = {KW(3755),KW(3756),KW(2704),KW(426),KW(430),KW(3757)};
static constexpr std::string_view EKW_1131[] = {KW(3758),KW(1692),KW(1687),KW(430),KW(3554)};
static constexpr std::string_view EKW_1132[] = {KW(3759),KW(3760),KW(1959),KW(3569),KW(3761),KW(3762)};
static constexpr std::string_view EKW_1133[] = {KW(3763),KW(3742),KW(3764),KW(3765),KW(814),KW(761),KW(3766),KW(647),KW(3767)};
static constexpr std::string_view EKW_1134[] = {KW(3768),KW(3742),KW(3764),KW(814),KW(3765),KW(761),KW(3766),KW(647),KW(93)};
static constexpr std::string_view EKW_1135[] = {KW(3769),KW(3742),KW(3764),KW(814),KW(3765),KW(761),KW(3766),KW(3770),KW(647)};
static constexpr std::string_view EKW_1136[] = {KW(3771),KW(3742),KW(3764),KW(814),KW(3765),KW(761),KW(3766),KW(3772),KW(647)};
static constexpr std::string_view EKW_1137[] = {KW(3773),KW(3774),KW(761),KW(3775),KW(647),KW(2699)};
static constexpr std::string_view EKW_1138[] = {KW(3776),KW(3742),KW(3764),KW(647),KW(649),KW(814),KW(761),KW(3766),KW(3027),KW(3777),KW(3778)};
static constexpr std::string_view EKW_1139[] = {KW(3779),KW(647),KW(649),KW(1486),KW(3780),KW(3781)};
static constexpr std::string_view EKW_1140[] = {KW(3782),KW(647),KW(3011),KW(381),KW(1535),KW(3766),KW(3783),KW(3027),KW(3784),KW(3785),KW(1448),KW(3786)};
static constexpr std::string_view EKW_1141[] = {KW(3787),KW(3788),KW(3789),KW(2608),KW(1535),KW(1387),KW(3790),KW(3027),KW(3791),KW(3792),KW(629),KW(3793),KW(3794),KW(3795),KW(3796)};
static constexpr std::string_view EKW_1142[] = {KW(3797),KW(3798),KW(1387),KW(3799),KW(3800),KW(1384)};
static constexpr std::string_view EKW_1143[] = {KW(3801),KW(2657),KW(1388),KW(1385),KW(3802),KW(1535),KW(1387),KW(3027),KW(3800),KW(1384),KW(3803)};
static constexpr std::string_view EKW_1144[] = {KW(3804),KW(3805),KW(3806),KW(3807),KW(3808),KW(3809)};
static constexpr std::string_view EKW_1145[] = {KW(3810),KW(3811),KW(3812),KW(3813),KW(3814)};
static constexpr std::string_view EKW_1146[] = {KW(3815),KW(1744),KW(3816),KW(3817),KW(3809)};
static constexpr std::string_view EKW_1147[] = {KW(3818),KW(3819),KW(3820),KW(3821),KW(3822),KW(3823),KW(2502)};
static constexpr std::string_view EKW_1148[] = {KW(434),KW(1064),KW(3262),KW(3824),KW(452),KW(454),KW(453),KW(1931),KW(18),KW(3825),KW(445)};
static constexpr std::string_view EKW_1149[] = {KW(3826),KW(1),KW(439),KW(3262),KW(779),KW(18),KW(445),KW(434)};
static constexpr std::string_view EKW_1150[] = {KW(3827),KW(30),KW(3828),KW(1456),KW(1931),KW(18),KW(3829),KW(3830)};
static constexpr std::string_view EKW_1151[] = {KW(3831),KW(347),KW(3832),KW(1348),KW(1351),KW(1931)};
static constexpr std::string_view EKW_1152[] = {KW(3833),KW(3828),KW(2921)};
static constexpr std::string_view EKW_1153[] = {KW(3834),KW(3835),KW(3836),KW(1503),KW(1064),KW(3837),KW(1335),KW(1931),KW(3822)};
static constexpr std::string_view EKW_1154[] = {KW(3838),KW(1064),KW(3837),KW(3839),KW(3835),KW(3840),KW(1931),KW(3841),KW(3842)};
static constexpr std::string_view EKW_1155[] = {KW(3843),KW(1064),KW(2936),KW(1931),KW(3844),KW(3845),KW(3846)};
static constexpr std::string_view EKW_1156[] = {KW(3847),KW(3848),KW(1737),KW(3849),KW(1931)};
static constexpr std::string_view EKW_1157[] = {KW(3850),KW(168),KW(1737),KW(3849),KW(1931),KW(879)};
static constexpr std::string_view EKW_1158[] = {KW(3851),KW(3852),KW(1931)};
static constexpr std::string_view EKW_1159[] = {KW(3577),KW(3853),KW(3849),KW(1931),KW(1849),KW(3854)};
static constexpr std::string_view EKW_1160[] = {KW(1689),KW(3849),KW(1064),KW(2936),KW(1931),KW(3855),KW(3856)};
static constexpr std::string_view EKW_1161[] = {KW(3568),KW(1689),KW(1064),KW(1155),KW(1147),KW(1448),KW(1931),KW(3857),KW(3855)};
static constexpr std::string_view EKW_1162[] = {KW(3858),KW(1689),KW(1931),KW(3859),KW(3860)};
static constexpr std::string_view EKW_1163[] = {KW(3861),KW(1064),KW(1837),KW(1931),KW(1501),KW(1764)};
static constexpr std::string_view EKW_1164[] = {KW(3862),KW(1931),KW(1837),KW(3863),KW(1501),KW(1764),KW(3864),KW(3865)};
static constexpr std::string_view EKW_1165[] = {KW(3866),KW(1931),KW(1837),KW(3845),KW(1501),KW(1764),KW(3864),KW(3865)};
static constexpr std::string_view EKW_1166[] = {KW(3867),KW(3829),KW(1147),KW(1148),KW(1135),KW(1064),KW(441),KW(440),KW(3868),KW(1931),KW(1764),KW(3864)};
static constexpr std::string_view EKW_1167[] = {KW(3869),KW(1064),KW(3870),KW(1147),KW(3871),KW(1931),KW(713),KW(3835),KW(3872),KW(3873)};
static constexpr std::string_view EKW_1168[] = {KW(3874),KW(3875),KW(34),KW(3876)};
static constexpr std::string_view EKW_1169[] = {KW(922),KW(1064),KW(3262),KW(164),KW(2925),KW(2936),KW(1931),KW(3877),KW(3878)};
static constexpr std::string_view EKW_1170[] = {KW(3879),KW(1064),KW(3880),KW(3262),KW(2936),KW(3881),KW(1931),KW(922),KW(3882)};
static constexpr std::string_view EKW_1171[] = {KW(3883),KW(3881),KW(3262),KW(2936),KW(1931),KW(3884),KW(155)};
static constexpr std::string_view EKW_1172[] = {KW(3870),KW(2950),KW(3885),KW(3886),KW(3881),KW(1842)};
static constexpr std::string_view EKW_1173[] = {KW(3887),KW(1278),KW(1280),KW(3881),KW(1535),KW(3888),KW(3889),KW(1283)};
static constexpr std::string_view EKW_1174[] = {KW(3890),KW(3138),KW(440),KW(441),KW(3891),KW(3892),KW(3893),KW(3894),KW(3895),KW(3896)};
static constexpr std::string_view EKW_1175[] = {KW(3897),KW(1064),KW(1144),KW(1988),KW(1931),KW(1689),KW(3898),KW(3899)};
static constexpr std::string_view EKW_1176[] = {KW(3900),KW(3901),KW(1723),KW(3902),KW(3903),KW(1931),KW(1676),KW(3904),KW(1719),KW(3646),KW(3905)};
static constexpr std::string_view EKW_1177[] = {KW(3906),KW(3907),KW(2861),KW(3908),KW(1931)};
static constexpr std::string_view EKW_1178[] = {KW(3909),KW(3910),KW(3911),KW(3912),KW(1931),KW(3873)};
static constexpr std::string_view EKW_1179[] = {KW(3913),KW(1064),KW(3901),KW(1147),KW(3914),KW(3915),KW(1931),KW(3916),KW(3917),KW(1148)};
static constexpr std::string_view EKW_1180[] = {KW(3918),KW(3901),KW(1064),KW(3919),KW(1931),KW(3920),KW(3894),KW(3921),KW(3872),KW(3873)};
static constexpr std::string_view EKW_1181[] = {KW(3922),KW(1687),KW(1931),KW(3923),KW(3921)};
static constexpr std::string_view EKW_1182[] = {KW(3924),KW(3901),KW(1064),KW(3925),KW(1931),KW(423),KW(3920),KW(885),KW(3926),KW(3921),KW(3872),KW(3873)};
static constexpr std::string_view EKW_1183[] = {KW(3927),KW(3928),KW(3929)};
static constexpr std::string_view EKW_1184[] = {KW(1469),KW(1470),KW(3930),KW(3931),KW(1464),KW(3932),KW(1931),KW(1473),KW(1442)};
static constexpr std::string_view EKW_1185[] = {KW(3933),KW(1064),KW(3262),KW(1147),KW(1182),KW(2082),KW(1052),KW(1931),KW(3934),KW(3872),KW(3873)};
static constexpr std::string_view EKW_1186[] = {KW(3935),KW(814),KW(3936),KW(1163),KW(1938),KW(1535),KW(1931),KW(3027),KW(1503),KW(1500),KW(3937),KW(3938)};
static constexpr std::string_view EKW_1187[] = {KW(3939),KW(1283),KW(3940),KW(3941),KW(3942),KW(1285),KW(1488),KW(424),KW(1422),KW(1279),KW(1280),KW(3943),KW(1535),KW(3135),KW(426),KW(1931),KW(1281),KW(2887),KW(3944)};
static constexpr std::string_view EKW_1188[] = {KW(3945),KW(1488),KW(3632),KW(1931),KW(424)};
static constexpr std::string_view EKW_1189[] = {KW(3946),KW(3611),KW(3828),KW(3947),KW(3948)};
static constexpr std::string_view EKW_1190[] = {KW(3949),KW(1461),KW(1453),KW(3950),KW(403),KW(1),KW(424),KW(119),KW(3951)};
static constexpr std::string_view EKW_1191[] = {KW(3952),KW(3953),KW(3954),KW(1931),KW(3955),KW(2970),KW(3956)};
static constexpr std::string_view EKW_1192[] = {KW(787),KW(1147),KW(1135),KW(1064),KW(1148),KW(1067),KW(3957),KW(3958),KW(3959),KW(3040)};
static constexpr std::string_view EKW_1193[] = {KW(893),KW(1499),KW(3960),KW(1498),KW(91),KW(3961),KW(1064),KW(3962),KW(3963),KW(3964),KW(3965),KW(698)};
static constexpr std::string_view EKW_1194[] = {KW(3966),KW(386),KW(3967),KW(3961),KW(3962),KW(3968),KW(3969),KW(698)};
static constexpr std::string_view EKW_1195[] = {KW(3970),KW(1093),KW(3971),KW(178),KW(176),KW(3972),KW(241),KW(3217),KW(243),KW(384)};
static constexpr std::string_view EKW_1196[] = {KW(3973),KW(1093),KW(3971),KW(178),KW(3974),KW(2501)};
static constexpr std::string_view EKW_1197[] = {KW(3975),KW(3971),KW(3976),KW(3974),KW(3977),KW(154),KW(3978),KW(1093),KW(867)};
static constexpr std::string_view EKW_1198[] = {KW(3979),KW(3971),KW(3980),KW(3981),KW(3976),KW(3974),KW(3027),KW(3982),KW(3983),KW(1093),KW(95),KW(3535)};
static constexpr std::string_view EKW_1199[] = {KW(3984),KW(3971),KW(1093),KW(3985),KW(3986),KW(3987),KW(2916),KW(3983),KW(3988),KW(3989),KW(3061),KW(3990)};
static constexpr std::string_view EKW_1200[] = {KW(3988),KW(1093),KW(3976),KW(3971),KW(3987),KW(3991),KW(2916),KW(3992)};
static constexpr std::string_view EKW_1201[] = {KW(3993),KW(3994),KW(1375),KW(3995),KW(2916),KW(3027),KW(2486),KW(3996)};
static constexpr std::string_view EKW_1202[] = {KW(3580),KW(1093),KW(3997),KW(1544),KW(1533),KW(3998),KW(3999),KW(4000),KW(1499)};
static constexpr std::string_view EKW_1203[] = {KW(4001),KW(1093),KW(3971),KW(241),KW(176),KW(243),KW(3972),KW(4002),KW(680),KW(4003),KW(242),KW(684),KW(4004),KW(3217),KW(685),KW(4000),KW(384)};
static constexpr std::string_view EKW_1204[] = {KW(4005),KW(4006),KW(4007),KW(1059),KW(1535),KW(3027),KW(1375),KW(4008)};
static constexpr std::string_view EKW_1205[] = {KW(4009),KW(789),KW(4010),KW(1093),KW(1535),KW(4011),KW(4012),KW(3027),KW(1375),KW(4013),KW(1849),KW(4014)};
static constexpr std::string_view EKW_1206[] = {KW(4015),KW(1375),KW(789),KW(1535),KW(3027),KW(715),KW(697),KW(4016)};
static constexpr std::string_view EKW_1207[] = {KW(4017),KW(1903),KW(4018),KW(1371),KW(4019),KW(4020),KW(1375),KW(4021)};
static constexpr std::string_view EKW_1208[] = {KW(4022),KW(4023),KW(1375)};
static constexpr std::string_view EKW_1209[] = {KW(4024),KW(4025),KW(1375)};
static constexpr std::string_view EKW_1210[] = {KW(4026),KW(1093),KW(1375),KW(4027),KW(1903),KW(4019),KW(1535),KW(3027),KW(4028),KW(4020),KW(4016)};
static constexpr std::string_view EKW_1211[] = {KW(4029),KW(1375),KW(789),KW(4030),KW(1535),KW(4031),KW(4032),KW(4033),KW(3027),KW(4034),KW(4035)};
static constexpr std::string_view EKW_1212[] = {KW(4036),KW(2916),KW(1375),KW(4021),KW(4037),KW(4038),KW(3027),KW(654),KW(4039)};
static constexpr std::string_view EKW_1213[] = {KW(4040),KW(1375),KW(3994),KW(1030),KW(4041),KW(1535),KW(3027),KW(4042)};
static constexpr std::string_view EKW_1214[] = {KW(4043),KW(1375),KW(4044),KW(703),KW(4045),KW(4046)};
static constexpr std::string_view EKW_1215[] = {KW(4047),KW(1375),KW(3994),KW(4048),KW(1535),KW(4049),KW(3051),KW(3027),KW(223)};
static constexpr std::string_view EKW_1216[] = {KW(4050),KW(4051),KW(3994),KW(1059),KW(1535),KW(3027),KW(1375)};
static constexpr std::string_view EKW_1217[] = {KW(4052),KW(1375),KW(4053),KW(1535),KW(3027),KW(429),KW(3994),KW(1030)};
static constexpr std::string_view EKW_1218[] = {KW(4054),KW(1375),KW(3994),KW(4055),KW(4056),KW(1535),KW(3027),KW(4057),KW(4058),KW(3808),KW(4059)};
static constexpr std::string_view EKW_1219[] = {KW(4060),KW(1375),KW(4061),KW(1535),KW(3027),KW(3994),KW(4062)};
static constexpr std::string_view EKW_1220[] = {KW(4063),KW(1375),KW(3994),KW(4064),KW(4065)};
static constexpr std::string_view EKW_1221[] = {KW(4066),KW(1375),KW(4067),KW(4068),KW(4069),KW(4070)};
static constexpr std::string_view EKW_1222[] = {KW(4071),KW(1375),KW(3994),KW(4072),KW(2614)};
static constexpr std::string_view EKW_1223[] = {KW(4073),KW(3559),KW(1375),KW(3994),KW(4074),KW(4075)};
static constexpr std::string_view EKW_1224[] = {KW(4076),KW(1375),KW(3994)};
static constexpr std::string_view EKW_1225[] = {KW(4077),KW(1363),KW(3263),KW(4030),KW(4025),KW(2289),KW(2916),KW(4078),KW(4079),KW(4080),KW(4081)};
static constexpr std::string_view EKW_1226[] = {KW(4082),KW(4078),KW(4083),KW(4084),KW(4085),KW(2289),KW(2916),KW(312),KW(4086),KW(4087),KW(1005),KW(4080)};
static constexpr std::string_view EKW_1227[] = {KW(4080),KW(1363),KW(2916),KW(4025),KW(761),KW(952),KW(4088)};
static constexpr std::string_view EKW_1228[] = {KW(4089),KW(1363),KW(2916),KW(4025),KW(4084),KW(4090),KW(952)};
static constexpr std::string_view EKW_1229[] = {KW(4091),KW(4092),KW(3256),KW(4093),KW(4094),KW(4095),KW(2916)};
static constexpr std::string_view EKW_1230[] = {KW(4096),KW(2916),KW(1363),KW(4097)};
static constexpr std::string_view EKW_1231[] = {KW(4098),KW(988),KW(4099),KW(4100),KW(4101),KW(952)};
static constexpr std::string_view EKW_1232[] = {KW(4102),KW(4103),KW(405),KW(4104),KW(4099),KW(242),KW(381)};
static constexpr std::string_view EKW_1233[] = {KW(4105),KW(4106),KW(988),KW(4107),KW(4108),KW(3234),KW(3515)};
static constexpr std::string_view EKW_1234[] = {KW(1361),KW(1363),KW(4109),KW(4110),KW(4111),KW(656),KW(4112),KW(4113),KW(4114),KW(4115)};
static constexpr std::string_view EKW_1235[] = {KW(4116),KW(1363),KW(4117),KW(4109),KW(4118)};
static constexpr std::string_view EKW_1236[] = {KW(4119),KW(2303),KW(4120),KW(656)};
static constexpr std::string_view EKW_1237[] = {KW(4121),KW(1363),KW(656),KW(4122),KW(4123),KW(846)};
static constexpr std::string_view EKW_1238[] = {KW(4124),KW(4125),KW(4126),KW(95)};
static constexpr std::string_view EKW_1239[] = {KW(4127),KW(1363),KW(4128),KW(656)};
static constexpr std::string_view EKW_1240[] = {KW(4129),KW(1363),KW(4130),KW(4131),KW(4132),KW(4093),KW(3027),KW(4133),KW(4134),KW(4135)};
static constexpr std::string_view EKW_1241[] = {KW(4136),KW(3256),KW(1363),KW(4137),KW(4093),KW(4138),KW(656)};
static constexpr std::string_view EKW_1242[] = {KW(4139),KW(1363),KW(4140),KW(4132),KW(4141),KW(4093),KW(4142),KW(4143),KW(656),KW(4144)};
static constexpr std::string_view EKW_1243[] = {KW(4140),KW(4142),KW(4132),KW(4141),KW(656),KW(3027),KW(4135),KW(4144),KW(654)};
static constexpr std::string_view EKW_1244[] = {KW(4145),KW(4146),KW(4147),KW(4148),KW(3799),KW(4149)};
static constexpr std::string_view EKW_1245[] = {KW(4150),KW(4151),KW(4130),KW(1535),KW(4152),KW(3027),KW(4153),KW(654)};
static constexpr std::string_view EKW_1246[] = {KW(4154),KW(4155),KW(4152),KW(3027),KW(4156)};
static constexpr std::string_view EKW_1247[] = {KW(4157),KW(654),KW(4158),KW(4130),KW(4155),KW(4152),KW(3027)};
static constexpr std::string_view EKW_1248[] = {KW(4159),KW(4155),KW(4151),KW(4130),KW(1535),KW(4160),KW(4161),KW(3027),KW(4162)};
static constexpr std::string_view EKW_1249[] = {KW(4163),KW(1363),KW(4037),KW(3256),KW(4164),KW(3027),KW(4165),KW(654)};
static constexpr std::string_view EKW_1250[] = {KW(1070),KW(4030),KW(3798),KW(4038),KW(3027),KW(2852),KW(654)};
static constexpr std::string_view EKW_1251[] = {KW(4166),KW(3798),KW(4030),KW(2852),KW(654),KW(4167)};
static constexpr std::string_view EKW_1252[] = {KW(4168),KW(4151),KW(4130),KW(4169),KW(3027)};
static constexpr std::string_view EKW_1253[] = {KW(4170),KW(4130),KW(654),KW(3256),KW(4093),KW(4138),KW(3027),KW(4158),KW(4171),KW(4172)};
static constexpr std::string_view EKW_1254[] = {KW(4173),KW(4174),KW(4175),KW(4176),KW(1427),KW(4177),KW(4178),KW(4179),KW(4086),KW(2813)};
static constexpr std::string_view EKW_1255[] = {KW(4180),KW(4174),KW(4175),KW(4176),KW(1427),KW(4177),KW(4178),KW(4179),KW(4086),KW(2813),KW(4181)};
static constexpr std::string_view EKW_1256[] = {KW(4182),KW(1411),KW(4183),KW(2151)};
static constexpr std::string_view EKW_1257[] = {KW(4184),KW(2151),KW(3515),KW(4185),KW(618),KW(3051)};
static constexpr std::string_view EKW_1258[] = {KW(4186),KW(779),KW(2861),KW(4187),KW(336),KW(3051),KW(2151),KW(2813),KW(4188)};
static constexpr std::string_view EKW_1259[] = {KW(4189),KW(2151),KW(2303),KW(605),KW(637),KW(1487),KW(2737),KW(4190),KW(1448)};
static constexpr std::string_view EKW_1260[] = {KW(4191),KW(4192),KW(4193)};
static constexpr std::string_view EKW_1261[] = {KW(4194),KW(4195),KW(4013),KW(4130),KW(2303),KW(4196),KW(4197),KW(4198)};
static constexpr std::string_view EKW_1262[] = {KW(4199),KW(4200),KW(4201),KW(4202),KW(4203),KW(1279),KW(381)};
static constexpr std::string_view EKW_1263[] = {KW(4204),KW(4197),KW(4200),KW(4201),KW(4202),KW(4205),KW(1279),KW(4196),KW(4206)};
static constexpr std::string_view EKW_1264[] = {KW(4207),KW(4200),KW(4201),KW(4202),KW(4196),KW(4203)};
static constexpr std::string_view EKW_1265[] = {KW(4208),KW(4200),KW(4201),KW(4202),KW(1279),KW(4196),KW(4203)};
static constexpr std::string_view EKW_1266[] = {KW(4209),KW(4200),KW(4201),KW(4202),KW(4203),KW(4196)};
static constexpr std::string_view EKW_1267[] = {KW(4210),KW(4205),KW(4201),KW(4196),KW(4197),KW(4211),KW(4212)};
static constexpr std::string_view EKW_1268[] = {KW(4113),KW(1057),KW(4130),KW(4013),KW(2303),KW(4196),KW(761),KW(4197),KW(4213),KW(119)};
static constexpr std::string_view EKW_1269[] = {KW(4214),KW(4013),KW(2303),KW(4215),KW(4197),KW(4216),KW(4113),KW(3494),KW(1183)};
static constexpr std::string_view EKW_1270[] = {KW(4217),KW(4218),KW(4219),KW(2303),KW(4220),KW(4221),KW(4222),KW(4223)};
static constexpr std::string_view EKW_1271[] = {KW(4224),KW(4218),KW(4225),KW(2878),KW(2303),KW(3941),KW(4222),KW(4226)};
static constexpr std::string_view EKW_1272[] = {KW(4227),KW(4218),KW(4219),KW(2303),KW(1225),KW(4222),KW(4228)};
static constexpr std::string_view EKW_1273[] = {KW(4229),KW(4230),KW(4231),KW(2916),KW(4232),KW(2303)};
static constexpr std::string_view EKW_1274[] = {KW(4233),KW(4230),KW(4231),KW(4234),KW(4232),KW(2303),KW(285)};
static constexpr std::string_view EKW_1275[] = {KW(4235),KW(4236),KW(4137),KW(4237),KW(4238),KW(735),KW(4239)};
static constexpr std::string_view EKW_1276[] = {KW(4240),KW(4236),KW(4241),KW(4137),KW(735),KW(4242),KW(4243)};
static constexpr std::string_view EKW_1277[] = {KW(4241),KW(4244),KW(4243)};
static constexpr std::string_view EKW_1278[] = {KW(4245),KW(163),KW(4246),KW(4247),KW(4244),KW(2507),KW(4248),KW(4249),KW(162)};
static constexpr std::string_view EKW_1279[] = {KW(3877),KW(164),KW(4250),KW(766),KW(949),KW(782),KW(4251)};
static constexpr std::string_view EKW_1280[] = {KW(4252),KW(164),KW(2925),KW(1448),KW(163),KW(4250),KW(2924),KW(4253),KW(4254),KW(697),KW(165)};
static constexpr std::string_view EKW_1281[] = {KW(4255),KW(164),KW(2925),KW(4254),KW(4250),KW(2966),KW(2924),KW(4253),KW(697),KW(165)};
static constexpr std::string_view EKW_1282[] = {KW(4256),KW(164),KW(2925),KW(163),KW(4250),KW(2924),KW(4253),KW(4254),KW(697),KW(165)};
static constexpr std::string_view EKW_1283[] = {KW(4257),KW(1437),KW(4258),KW(164),KW(2925),KW(4259),KW(1440),KW(4260),KW(4250),KW(2924),KW(4253),KW(4254),KW(697),KW(4261),KW(165),KW(4262)};
static constexpr std::string_view EKW_1284[] = {KW(4263),KW(163),KW(4259),KW(4246),KW(4244),KW(2924),KW(4264),KW(4254),KW(1394),KW(2162),KW(4265),KW(697)};
static constexpr std::string_view EKW_1285[] = {KW(4266),KW(164),KW(2925),KW(163),KW(4254),KW(4246),KW(2936),KW(4267),KW(2924),KW(1745),KW(4268),KW(4269),KW(4270),KW(4271)};
static constexpr std::string_view EKW_1286[] = {KW(4272),KW(4273),KW(4274),KW(4275),KW(4276),KW(4277)};
static constexpr std::string_view EKW_1287[] = {KW(4278),KW(4279),KW(4280),KW(4281),KW(4282),KW(2924),KW(4250),KW(4283),KW(4284),KW(4285),KW(164),KW(4286),KW(2914),KW(165),KW(4287),KW(4288),KW(4289)};
static constexpr std::string_view EKW_1288[] = {KW(695),KW(4290),KW(2917),KW(4291),KW(2916),KW(694),KW(4292)};
static constexpr std::string_view EKW_1289[] = {KW(4293),KW(2916),KW(4291),KW(694),KW(4290),KW(801)};
static constexpr std::string_view EKW_1290[] = {KW(4294),KW(694),KW(4291),KW(2916),KW(828),KW(4290),KW(4295),KW(696),KW(4087)};
static constexpr std::string_view EKW_1291[] = {KW(4296),KW(694),KW(2916),KW(730),KW(314),KW(4297),KW(4298),KW(4290),KW(696),KW(4299),KW(4300)};
static constexpr std::string_view EKW_1292[] = {KW(4301),KW(4291),KW(694),KW(703),KW(2916),KW(4290),KW(696),KW(4300)};
static constexpr std::string_view EKW_1293[] = {KW(4302),KW(694),KW(4218),KW(703),KW(2916),KW(4290),KW(696),KW(4087)};
static constexpr std::string_view EKW_1294[] = {KW(4303),KW(696),KW(706),KW(4304),KW(703),KW(4305),KW(2916),KW(4306),KW(4307),KW(2678)};
static constexpr std::string_view EKW_1295[] = {KW(4308),KW(694),KW(4291),KW(2916),KW(696),KW(4309)};
static constexpr std::string_view EKW_1296[] = {KW(4310),KW(694),KW(2916),KW(4291),KW(696),KW(4309)};
static constexpr std::string_view EKW_1297[] = {KW(4311),KW(694),KW(4291),KW(2916),KW(696),KW(4309)};
static constexpr std::string_view EKW_1298[] = {KW(4312),KW(694),KW(4291),KW(2916),KW(696),KW(242),KW(4309)};
static constexpr std::string_view EKW_1299[] = {KW(4309),KW(694),KW(4290),KW(695),KW(2916),KW(696),KW(4313)};
static constexpr std::string_view EKW_1300[] = {KW(4314),KW(4315),KW(4316),KW(4317)};
static constexpr std::string_view EKW_1301[] = {KW(4318),KW(1057),KW(1058),KW(2303),KW(4319),KW(1283),KW(4196),KW(4320),KW(4321),KW(4322)};
static constexpr std::string_view EKW_1302[] = {KW(4323),KW(4324),KW(1057),KW(4319),KW(1058),KW(2991),KW(4325)};
static constexpr std::string_view EKW_1303[] = {KW(4326),KW(1057),KW(4319),KW(1058),KW(2916),KW(312),KW(4327)};
static constexpr std::string_view EKW_1304[] = {KW(4324),KW(1057),KW(4319),KW(1058),KW(4328),KW(2916),KW(312),KW(4327)};
static constexpr std::string_view EKW_1305[] = {KW(4329),KW(4330),KW(1386),KW(1387),KW(4331),KW(2916),KW(312),KW(4327),KW(1384)};
static constexpr std::string_view EKW_1306[] = {KW(4332),KW(4330),KW(1386),KW(2916),KW(312),KW(4327)};
static constexpr std::string_view EKW_1307[] = {KW(4333),KW(1058),KW(4218),KW(1057),KW(4318),KW(2303),KW(4319),KW(1422),KW(793),KW(794),KW(795),KW(4196),KW(1059),KW(2916),KW(4222),KW(4334),KW(697),KW(4335)};
static constexpr std::string_view EKW_1308[] = {KW(4336),KW(1335),KW(4218),KW(551),KW(1294),KW(1422),KW(4337),KW(4338),KW(3253)};
static constexpr std::string_view EKW_1309[] = {KW(4339),KW(4218),KW(1335),KW(4219),KW(29),KW(4340),KW(4341)};
static constexpr std::string_view EKW_1310[] = {KW(4342),KW(4218),KW(4343)};
static constexpr std::string_view EKW_1311[] = {KW(4344),KW(4345),KW(1335),KW(1057)};
static constexpr std::string_view EKW_1312[] = {KW(4346),KW(3284),KW(1845),KW(4347),KW(775),KW(4348),KW(2823)};
static constexpr std::string_view EKW_1313[] = {KW(4349),KW(3284),KW(1845),KW(4350),KW(4347),KW(1229)};
static constexpr std::string_view EKW_1314[] = {KW(4351),KW(4333),KW(1057),KW(697),KW(4352)};
static constexpr std::string_view EKW_1315[] = {KW(4353),KW(1845),KW(3284),KW(4350),KW(4352)};
static constexpr std::string_view EKW_1316[] = {KW(4354),KW(1335),KW(1057),KW(4355),KW(3990)};
static constexpr std::string_view EKW_1317[] = {KW(4356),KW(4280),KW(4281),KW(4282),KW(4357),KW(1335),KW(4358),KW(164),KW(2925),KW(935),KW(4359),KW(4284),KW(4360),KW(4086),KW(4361),KW(4287),KW(373),KW(4288),KW(4289),KW(4362)};
static constexpr std::string_view EKW_1318[] = {KW(4363),KW(4280),KW(4281),KW(4282),KW(4364),KW(1335),KW(4358),KW(164),KW(2925),KW(624),KW(4365),KW(314),KW(4297),KW(4366),KW(4360),KW(4367),KW(4287)};
static constexpr std::string_view EKW_1319[] = {KW(4368),KW(4280),KW(4281),KW(4282),KW(4360)};
static constexpr std::string_view EKW_1320[] = {KW(4369),KW(1057),KW(4218)};
static constexpr std::string_view EKW_1321[] = {KW(4370),KW(1057),KW(735),KW(4371),KW(2841),KW(3653),KW(4372),KW(204)};
static constexpr std::string_view EKW_1322[] = {KW(4373),KW(1057),KW(2841),KW(4374),KW(4371),KW(4375),KW(3653),KW(381)};
static constexpr std::string_view EKW_1323[] = {KW(4376),KW(4218),KW(1057),KW(4377)};
static constexpr std::string_view EKW_1324[] = {KW(4378),KW(4218),KW(1057),KW(2916),KW(4379),KW(4376)};
static constexpr std::string_view EKW_1325[] = {KW(4380),KW(1057),KW(4381),KW(4382),KW(4149),KW(1283),KW(4330),KW(1339),KW(4383),KW(4384)};
static constexpr std::string_view EKW_1326[] = {KW(4385),KW(1057),KW(4149),KW(1339),KW(4383),KW(4386),KW(4387)};
static constexpr std::string_view EKW_1327[] = {KW(4388),KW(1057),KW(2529),KW(761),KW(1617),KW(2813)};
static constexpr std::string_view EKW_1328[] = {KW(4389),KW(1335),KW(1057),KW(4390)};
static constexpr std::string_view EKW_1329[] = {KW(4391),KW(4392),KW(4345)};
static constexpr std::string_view EKW_1330[] = {KW(4393),KW(4394),KW(4395),KW(4396),KW(4397),KW(4398),KW(4399),KW(2617),KW(4400),KW(4401)};
static constexpr std::string_view EKW_1331[] = {KW(4402),KW(4403),KW(4404),KW(4405),KW(29),KW(4406),KW(1430),KW(4407)};
static constexpr std::string_view EKW_1332[] = {KW(4408),KW(4407),KW(4403),KW(4406),KW(13),KW(4405),KW(4409)};
static constexpr std::string_view EKW_1333[] = {KW(4410),KW(4403),KW(67),KW(2991),KW(4120),KW(4406),KW(4411),KW(4412),KW(4407)};
static constexpr std::string_view EKW_1334[] = {KW(4413),KW(4403),KW(4407),KW(29),KW(4406),KW(4414),KW(67)};
static constexpr std::string_view EKW_1335[] = {KW(4415),KW(4406),KW(4416),KW(4404),KW(766)};
static constexpr std::string_view EKW_1336[] = {KW(4417),KW(4406),KW(4416),KW(4404),KW(3750)};
static constexpr std::string_view EKW_1337[] = {KW(4418),KW(4419),KW(1453),KW(4420),KW(4421),KW(4422),KW(2813)};
static constexpr std::string_view EKW_1338[] = {KW(4423),KW(2813),KW(2530),KW(2529),KW(4424),KW(894),KW(2341)};
static constexpr std::string_view EKW_1339[] = {KW(4425),KW(4419),KW(4426),KW(4427),KW(4428),KW(2813)};
static constexpr std::string_view EKW_1340[] = {KW(4429),KW(4419),KW(1453),KW(4420),KW(2813)};
static constexpr std::string_view EKW_1341[] = {KW(4430),KW(4419),KW(1453),KW(4420),KW(4431),KW(2813)};
static constexpr std::string_view EKW_1342[] = {KW(4432),KW(2810),KW(2808)};
static constexpr std::string_view EKW_1343[] = {KW(4433),KW(2810)};
static constexpr std::string_view EKW_1344[] = {KW(806),KW(809),KW(807),KW(420),KW(4434),KW(618)};
static constexpr std::string_view EKW_1345[] = {KW(4435),KW(2810),KW(2110),KW(4436),KW(4437)};
static constexpr std::string_view EKW_1346[] = {KW(4438),KW(1723),KW(4439),KW(3701),KW(4440),KW(2813),KW(1152)};
static constexpr std::string_view EKW_1347[] = {KW(4441),KW(3828),KW(4403),KW(2810)};
static constexpr std::string_view EKW_1348[] = {KW(4442),KW(2529),KW(2530),KW(4443),KW(879),KW(2896),KW(2813)};
static constexpr std::string_view EKW_1349[] = {KW(1325),KW(4419),KW(4444),KW(4445),KW(4446),KW(4447),KW(4431),KW(2813)};
static constexpr std::string_view EKW_1350[] = {KW(4448),KW(4419),KW(4449),KW(2813)};
static constexpr std::string_view EKW_1351[] = {KW(4450),KW(4451),KW(4419),KW(4446),KW(4449),KW(2813)};
static constexpr std::string_view EKW_1352[] = {KW(4452),KW(4453),KW(4454),KW(2813)};
static constexpr std::string_view EKW_1353[] = {KW(4455),KW(2813),KW(4456),KW(4457),KW(3665),KW(4458),KW(4459)};
static constexpr std::string_view EKW_1354[] = {KW(4460),KW(1294),KW(4461),KW(1779),KW(1296),KW(4462),KW(1297),KW(2813),KW(1152)};
static constexpr std::string_view EKW_1355[] = {KW(4463),KW(1083),KW(679),KW(119)};
static constexpr std::string_view EKW_1356[] = {KW(4379),KW(4464),KW(4465),KW(4466),KW(4467),KW(4468),KW(801)};
static constexpr std::string_view EKW_1357[] = {KW(4469),KW(4470),KW(740)};
static constexpr std::string_view EKW_1358[] = {KW(4471),KW(4406),KW(1424),KW(4466)};
static constexpr std::string_view EKW_1359[] = {KW(4472),KW(4419),KW(908),KW(4473),KW(4474),KW(4475),KW(2105),KW(955),KW(4476),KW(2813)};
static constexpr std::string_view EKW_1360[] = {KW(4477),KW(4419),KW(4444),KW(4446),KW(4447),KW(1318),KW(4478),KW(2813)};
static constexpr std::string_view EKW_1361[] = {KW(4479),KW(4480),KW(2848),KW(4481)};
static constexpr std::string_view EKW_1362[] = {KW(4482),KW(4419),KW(4483),KW(4484),KW(4485),KW(2813)};
static constexpr std::string_view EKW_1363[] = {KW(4486),KW(2813),KW(4426)};
static constexpr std::string_view EKW_1364[] = {KW(4487),KW(4488),KW(4489),KW(3832),KW(1349),KW(2813)};
static constexpr std::string_view EKW_1365[] = {KW(4490),KW(1349),KW(3832),KW(1353),KW(4489),KW(1351),KW(795)};
static constexpr std::string_view EKW_1366[] = {KW(4491),KW(2287),KW(4492),KW(2877),KW(1353),KW(1350)};
static constexpr std::string_view EKW_1367[] = {KW(4493),KW(1350),KW(4494),KW(4495),KW(4496),KW(4497),KW(4498),KW(4499)};
static constexpr std::string_view EKW_1368[] = {KW(4500),KW(4501),KW(3832),KW(4502),KW(4489),KW(4196),KW(4503),KW(4504),KW(2813)};
static constexpr std::string_view EKW_1369[] = {KW(4505),KW(1406),KW(652),KW(4175),KW(4489),KW(4506),KW(4507),KW(2813)};
static constexpr std::string_view EKW_1370[] = {KW(4508),KW(2916),KW(4509),KW(4036),KW(652),KW(4510),KW(3174)};
static constexpr std::string_view EKW_1371[] = {KW(4511),KW(1107),KW(1269),KW(4512),KW(1573),KW(348),KW(3805),KW(347),KW(1272),KW(2765),KW(342),KW(2813),KW(4513),KW(4514)};
static constexpr std::string_view EKW_1372[] = {KW(4515),KW(4516),KW(358),KW(4517),KW(4518),KW(4519),KW(347),KW(4520),KW(360),KW(348),KW(4521)};
static constexpr std::string_view EKW_1373[] = {KW(4522),KW(1107),KW(348),KW(347),KW(4523),KW(4524),KW(4525),KW(4512),KW(342),KW(4526)};
static constexpr std::string_view EKW_1374[] = {KW(4527),KW(4528),KW(3950),KW(4529),KW(347),KW(348),KW(4530)};
static constexpr std::string_view EKW_1375[] = {KW(4531),KW(1083),KW(4532),KW(3950),KW(4533),KW(1659),KW(358),KW(1663),KW(1655)};
static constexpr std::string_view EKW_1376[] = {KW(4534),KW(1107),KW(347),KW(97),KW(348),KW(4535)};
static constexpr std::string_view EKW_1377[] = {KW(4536),KW(599),KW(348),KW(4537),KW(347),KW(2922),KW(4538),KW(1215)};
static constexpr std::string_view EKW_1378[] = {KW(4416),KW(2333),KW(4539),KW(4540),KW(4541),KW(3062)};
static constexpr std::string_view EKW_1379[] = {KW(4542),KW(905),KW(1083),KW(4543)};
static constexpr std::string_view EKW_1380[] = {KW(4544),KW(4545),KW(4546),KW(4547)};
static constexpr std::string_view EKW_1381[] = {KW(4548),KW(4549),KW(827),KW(3799),KW(2992),KW(2759),KW(4550),KW(4551),KW(3008)};
static constexpr std::string_view EKW_1382[] = {KW(1840),KW(330),KW(326),KW(4552),KW(1842)};
static constexpr std::string_view EKW_1383[] = {KW(4553),KW(4200),KW(4554),KW(1842),KW(4555),KW(4556),KW(4557)};
static constexpr std::string_view EKW_1384[] = {KW(3230),KW(3229),KW(4558),KW(3228)};
static constexpr std::string_view EKW_1385[] = {KW(4559),KW(4560),KW(4561),KW(4562),KW(1836),KW(4563),KW(4564)};
static constexpr std::string_view EKW_1386[] = {KW(4565),KW(4559),KW(2732),KW(4566),KW(1321),KW(4567)};
static constexpr std::string_view EKW_1387[] = {KW(253),KW(1835),KW(819),KW(1836),KW(3022),KW(261)};
static constexpr std::string_view EKW_1388[] = {KW(1838),KW(1835),KW(253),KW(1836),KW(3022),KW(328)};
static constexpr std::string_view EKW_1389[] = {KW(4568),KW(2506),KW(4569),KW(4570),KW(2074),KW(4065)};
static constexpr std::string_view EKW_1390[] = {KW(3133),KW(2529),KW(4571),KW(4572)};
static constexpr std::string_view EKW_1391[] = {KW(4573),KW(4574),KW(4575),KW(4576)};
static constexpr std::string_view EKW_1392[] = {KW(4577),KW(4578),KW(4579),KW(223)};
static constexpr std::string_view EKW_1393[] = {KW(4580),KW(4581),KW(4582),KW(1562),KW(4331),KW(4583)};
static constexpr std::string_view EKW_1394[] = {KW(4399),KW(4584),KW(1303),KW(2389)};
static constexpr std::string_view EKW_1395[] = {KW(4585),KW(285),KW(4559),KW(4586)};
static constexpr std::string_view EKW_1396[] = {KW(4587),KW(819),KW(2678),KW(4588),KW(2679),KW(4589)};
static constexpr std::string_view EKW_1397[] = {KW(4590),KW(2737),KW(1837),KW(4581),KW(4591),KW(4592)};
static constexpr std::string_view EKW_1398[] = {KW(4593),KW(4590),KW(1686),KW(4594),KW(2741),KW(4595),KW(1835),KW(4596)};
static constexpr std::string_view EKW_1399[] = {KW(4597),KW(4598),KW(4599),KW(1836),KW(4331),KW(1835),KW(288)};
static constexpr std::string_view EKW_1400[] = {KW(4600),KW(4601),KW(4581),KW(4602)};
static constexpr std::string_view EKW_1401[] = {KW(4603),KW(4604),KW(4605)};
static constexpr std::string_view EKW_1402[] = {KW(4606),KW(3085)};
static constexpr std::string_view EKW_1403[] = {KW(4607),KW(4608),KW(4609),KW(4610),KW(831),KW(1535),KW(4611),KW(801)};
static constexpr std::string_view EKW_1404[] = {KW(4612),KW(1572),KW(405),KW(4613),KW(601),KW(4614),KW(4615),KW(4616),KW(4617),KW(4618),KW(703),KW(3530)};
static constexpr std::string_view EKW_1405[] = {KW(4619),KW(601),KW(4614),KW(4620),KW(4616),KW(4615),KW(605),KW(4621)};
static constexpr std::string_view EKW_1406[] = {KW(4622),KW(405),KW(4613),KW(601),KW(4614),KW(4623),KW(2818)};
static constexpr std::string_view EKW_1407[] = {KW(4624),KW(4625),KW(4626),KW(4627),KW(591),KW(18),KW(4628)};
static constexpr std::string_view EKW_1408[] = {KW(4629),KW(2970),KW(3828),KW(4630),KW(4631),KW(879),KW(4632),KW(4633)};
static constexpr std::string_view EKW_1409[] = {KW(4634),KW(223),KW(4635),KW(4636),KW(1),KW(1195),KW(4637),KW(4638),KW(2890)};
static constexpr std::string_view EKW_1410[] = {KW(4639),KW(3986),KW(4640),KW(2327),KW(4641),KW(3996),KW(4642),KW(165)};
static constexpr std::string_view EKW_1411[] = {KW(4643),KW(4222),KW(4644),KW(4645),KW(4223),KW(4403)};
static constexpr std::string_view EKW_1412[] = {KW(4646),KW(164),KW(2925),KW(2926),KW(3068),KW(4246),KW(2924),KW(4647),KW(657),KW(4648)};
static constexpr std::string_view EKW_1413[] = {KW(4649),KW(3068),KW(165),KW(1195),KW(849),KW(4650),KW(4651),KW(1024),KW(2872),KW(4652),KW(801),KW(4395)};
static constexpr std::string_view EKW_1414[] = {KW(4653),KW(3068),KW(63),KW(4560),KW(4581),KW(3532),KW(2718),KW(4654),KW(801),KW(4655),KW(4656),KW(4657)};
static constexpr std::string_view EKW_1415[] = {KW(4658),KW(3068),KW(4002),KW(1083),KW(4659),KW(4660),KW(1836)};
static constexpr std::string_view EKW_1416[] = {KW(4661),KW(4559),KW(4560),KW(4561),KW(3068),KW(1140),KW(1144),KW(4662),KW(1143),KW(4663),KW(4664),KW(801)};
static constexpr std::string_view EKW_1417[] = {KW(4665),KW(727),KW(1148),KW(1147),KW(4559),KW(4564),KW(4560),KW(1140),KW(4662),KW(801),KW(4561),KW(4666),KW(4667),KW(3882)};
static constexpr std::string_view EKW_1418[] = {KW(4560),KW(3068),KW(4559),KW(4668),KW(4561),KW(1140),KW(1836),KW(4662),KW(165)};
static constexpr std::string_view EKW_1419[] = {KW(4669),KW(4670),KW(1133),KW(4671),KW(4672),KW(4673),KW(3074)};
static constexpr std::string_view EKW_1420[] = {KW(4674),KW(4559),KW(4560),KW(3068),KW(4662),KW(4561)};
static constexpr std::string_view EKW_1421[] = {KW(4675),KW(4676),KW(3068),KW(4677),KW(4678),KW(4679),KW(4680)};
static constexpr std::string_view EKW_1422[] = {KW(4681),KW(4682),KW(4677),KW(3068)};
static constexpr std::string_view EKW_1423[] = {KW(4683),KW(3068),KW(3214),KW(3158)};
static constexpr std::string_view EKW_1424[] = {KW(4684),KW(3068),KW(2842),KW(4685),KW(4686),KW(4415),KW(4402),KW(4687),KW(3253)};
static constexpr std::string_view EKW_1425[] = {KW(3172),KW(4688),KW(1460),KW(3162),KW(3163),KW(4689),KW(4690),KW(165),KW(801)};
static constexpr std::string_view EKW_1426[] = {KW(4691),KW(1283),KW(3172),KW(603),KW(165),KW(3176),KW(4692),KW(1133),KW(4693),KW(1625),KW(4694),KW(4695),KW(4696)};
static constexpr std::string_view EKW_1427[] = {KW(4697),KW(3257),KW(4403),KW(4407),KW(624),KW(1213),KW(882),KW(815),KW(680),KW(684),KW(685),KW(4694)};
static constexpr std::string_view EKW_1428[] = {KW(685),KW(4698),KW(882),KW(3257),KW(1213),KW(4699),KW(815),KW(4700),KW(4701),KW(4702),KW(404),KW(4539),KW(680),KW(242),KW(684),KW(381),KW(4703),KW(165)};
static constexpr std::string_view EKW_1429[] = {KW(4704),KW(4705),KW(1784),KW(1785),KW(1786),KW(685),KW(815),KW(680),KW(684),KW(165),KW(1758)};
static constexpr std::string_view EKW_1430[] = {KW(4706),KW(4607),KW(3068),KW(1098),KW(831),KW(680),KW(684),KW(685),KW(165),KW(801)};
static constexpr std::string_view EKW_1431[] = {KW(4707),KW(4395),KW(4394),KW(4397),KW(815),KW(4708),KW(680),KW(4400),KW(684),KW(685),KW(801)};
static constexpr std::string_view EKW_1432[] = {KW(4709),KW(2718),KW(3532),KW(4655),KW(815),KW(4654),KW(680),KW(242),KW(684),KW(685),KW(801)};
static constexpr std::string_view EKW_1433[] = {KW(4710),KW(4711),KW(4712),KW(1602),KW(815),KW(680),KW(684),KW(1625),KW(1710),KW(685)};
static constexpr std::string_view EKW_1434[] = {KW(4713),KW(4078),KW(241),KW(815),KW(2289),KW(2916),KW(680),KW(684),KW(952),KW(685),KW(4714),KW(4080)};
static constexpr std::string_view EKW_1435[] = {KW(4715),KW(4716),KW(2718),KW(2750),KW(336),KW(4717),KW(815),KW(4718),KW(680),KW(684),KW(4719),KW(685),KW(4720),KW(801),KW(4721)};
static constexpr std::string_view EKW_1436[] = {KW(4722),KW(4723),KW(603),KW(2825),KW(4724),KW(165),KW(801)};
static constexpr std::string_view EKW_1437[] = {KW(4725),KW(603),KW(165)};
static constexpr std::string_view EKW_1438[] = {KW(4726),KW(3068),KW(4727),KW(3044),KW(954),KW(761),KW(4728),KW(4729),KW(4086),KW(4289),KW(4730)};
static constexpr std::string_view EKW_1439[] = {KW(4731),KW(3068),KW(955),KW(954),KW(4732),KW(4733),KW(2830),KW(4734),KW(4729),KW(4735)};
static constexpr std::string_view EKW_1440[] = {KW(4736),KW(3068),KW(4737),KW(761),KW(4728),KW(954),KW(2830),KW(4086),KW(1005),KW(4738)};
static constexpr std::string_view EKW_1441[] = {KW(4739),KW(3068),KW(954),KW(4732),KW(4740),KW(2830),KW(4734),KW(4327),KW(4738),KW(4741)};
static constexpr std::string_view EKW_1442[] = {KW(4742),KW(3068),KW(954),KW(2410),KW(761),KW(4728),KW(4297),KW(4743),KW(4086),KW(4741),KW(4744)};
static constexpr std::string_view EKW_1443[] = {KW(4745),KW(3068),KW(954),KW(4732),KW(4746),KW(4734),KW(4747),KW(4327),KW(4741),KW(4748)};
static constexpr std::string_view EKW_1444[] = {KW(4749),KW(3068),KW(4750),KW(4751),KW(761),KW(4728),KW(954),KW(1003),KW(4747),KW(4086),KW(4748)};
static constexpr std::string_view EKW_1445[] = {KW(4752),KW(3068),KW(955),KW(954),KW(4732),KW(4753),KW(4734),KW(4747),KW(4729),KW(4735),KW(4748)};
static constexpr std::string_view EKW_1446[] = {KW(4754),KW(3068),KW(954),KW(4755),KW(4756),KW(4757),KW(4734),KW(4753)};
static constexpr std::string_view EKW_1447[] = {KW(4758),KW(4759),KW(954),KW(4760),KW(4761),KW(4757),KW(4762)};
static constexpr std::string_view EKW_1448[] = {KW(4763),KW(4751),KW(4764),KW(3068),KW(4765),KW(4766),KW(4767),KW(694),KW(4472),KW(1003),KW(4768)};
static constexpr std::string_view EKW_1449[] = {KW(4769),KW(3068),KW(4764),KW(4770),KW(954),KW(694),KW(4771),KW(4472),KW(1005),KW(4772)};
static constexpr std::string_view EKW_1450[] = {KW(4773),KW(3068),KW(954),KW(3044),KW(4774),KW(4086),KW(1005),KW(4775),KW(4289)};
static constexpr std::string_view EKW_1451[] = {KW(4776),KW(3068),KW(954),KW(2410),KW(4767),KW(4297),KW(4774),KW(4086),KW(1005),KW(4775)};
static constexpr std::string_view EKW_1452[] = {KW(4777),KW(4778),KW(3126),KW(1014),KW(4779),KW(4780),KW(815),KW(4297),KW(13),KW(4781),KW(4289)};
static constexpr std::string_view EKW_1453[] = {KW(4782),KW(3068),KW(4778),KW(3126),KW(4783),KW(4780),KW(815),KW(4297),KW(13),KW(4668),KW(4770),KW(4784),KW(4289),KW(4785)};
static constexpr std::string_view EKW_1454[] = {KW(4786),KW(4780),KW(839),KW(4764),KW(730),KW(1003)};
static constexpr std::string_view EKW_1455[] = {KW(4787),KW(839),KW(4780),KW(730),KW(1003)};
static constexpr std::string_view EKW_1456[] = {KW(4788),KW(4780),KW(839),KW(730),KW(4688),KW(312),KW(735),KW(4789),KW(897)};
static constexpr std::string_view EKW_1457[] = {KW(4790),KW(4780),KW(839),KW(730),KW(1005)};
static constexpr std::string_view EKW_1458[] = {KW(4791),KW(839),KW(3068),KW(730),KW(373),KW(4289)};
static constexpr std::string_view EKW_1459[] = {KW(4792),KW(2970),KW(2962),KW(2981),KW(1033),KW(2903),KW(3954)};
static constexpr std::string_view EKW_1460[] = {KW(4793),KW(4489),KW(4794),KW(1349),KW(4795)};
static constexpr std::string_view EKW_1461[] = {KW(4796),KW(1482),KW(2307),KW(4797),KW(4798),KW(4799),KW(4800),KW(4801),KW(4802),KW(2970),KW(801)};
static constexpr std::string_view EKW_1462[] = {KW(4803),KW(2980),KW(2983),KW(2982),KW(4804),KW(2970)};
static constexpr std::string_view EKW_1463[] = {KW(4805),KW(1482),KW(2307),KW(4797),KW(4798),KW(4806),KW(4807),KW(2970)};
static constexpr std::string_view EKW_1464[] = {KW(4808),KW(1818),KW(2970),KW(4809),KW(4810)};
static constexpr std::string_view EKW_1465[] = {KW(4811),KW(4812),KW(2972),KW(2970)};
static constexpr std::string_view EKW_1466[] = {KW(4813),KW(4814),KW(2970),KW(2972)};
static constexpr std::string_view EKW_1467[] = {KW(4815),KW(2974),KW(1478),KW(2970)};
static constexpr std::string_view EKW_1468[] = {KW(4816),KW(4817),KW(165)};
static constexpr std::string_view EKW_1469[] = {KW(4818),KW(4819),KW(2697),KW(2982),KW(4820),KW(4821),KW(4822),KW(4823),KW(4824),KW(2970)};
static constexpr std::string_view EKW_1470[] = {KW(4825),KW(727),KW(2970),KW(2982),KW(4826),KW(733),KW(4827),KW(890)};
static constexpr std::string_view EKW_1471[] = {KW(4828),KW(4829),KW(2970)};
static constexpr std::string_view EKW_1472[] = {KW(2040),KW(165),KW(727),KW(1152),KW(4830),KW(2038)};
static constexpr std::string_view EKW_1473[] = {KW(2024),KW(727),KW(165),KW(1152),KW(4830),KW(2020),KW(2012)};
static constexpr std::string_view EKW_1474[] = {KW(4831),KW(165),KW(1152),KW(727),KW(4830),KW(4832)};
static constexpr std::string_view EKW_1475[] = {KW(2230),KW(165),KW(1152),KW(727),KW(4830),KW(2228)};
static constexpr std::string_view EKW_1476[] = {KW(1978),KW(165),KW(727),KW(1152),KW(4830),KW(1977)};
static constexpr std::string_view EKW_1477[] = {KW(1151),KW(165),KW(1152),KW(727),KW(4830),KW(1149),KW(1150)};
static constexpr std::string_view EKW_1478[] = {KW(4462),KW(165),KW(727),KW(1152),KW(4830),KW(1818),KW(1296),KW(1297)};
static constexpr std::string_view EKW_1479[] = {KW(2275),KW(165),KW(1152),KW(727),KW(4830),KW(2274),KW(2276)};
static constexpr std::string_view EKW_1480[] = {KW(4440),KW(165),KW(1152),KW(727),KW(4830),KW(4439)};
static constexpr std::string_view EKW_1481[] = {KW(2046),KW(165),KW(1152),KW(727),KW(4830),KW(2045)};
static constexpr std::string_view EKW_1482[] = {KW(2819),KW(165),KW(727),KW(1152),KW(4830),KW(2188),KW(819)};
static constexpr std::string_view EKW_1483[] = {KW(2211),KW(727),KW(165),KW(1152),KW(4830),KW(2206)};
static constexpr std::string_view EKW_1484[] = {KW(2189),KW(165),KW(727),KW(4833),KW(4830),KW(2188),KW(2190),KW(2185),KW(1152)};
static constexpr std::string_view EKW_1485[] = {KW(4834),KW(3068),KW(4835),KW(1375),KW(3745),KW(4780),KW(4757),KW(404),KW(1005),KW(801),KW(2499),KW(4836)};
static constexpr std::string_view EKW_1486[] = {KW(4837),KW(4838),KW(4130),KW(4780),KW(4757),KW(815),KW(4839),KW(1003),KW(13),KW(4840),KW(1005),KW(801)};
static constexpr std::string_view EKW_1487[] = {KW(4841),KW(3068),KW(4838),KW(4780),KW(4757),KW(815),KW(4842),KW(4839),KW(1003),KW(4843),KW(4844),KW(154),KW(13),KW(4845),KW(1005),KW(801),KW(4846)};
static constexpr std::string_view EKW_1488[] = {KW(4847),KW(3068),KW(897),KW(954),KW(649),KW(4780),KW(761),KW(4771),KW(4086),KW(4848),KW(4387)};
static constexpr std::string_view EKW_1489[] = {KW(4849),KW(3068),KW(649),KW(2000),KW(4727),KW(4780),KW(761),KW(4496),KW(4086),KW(897),KW(801),KW(4387)};
static constexpr std::string_view EKW_1490[] = {KW(4850),KW(4771),KW(4737),KW(3068),KW(4780),KW(2737),KW(761),KW(4496),KW(4086),KW(897),KW(3587),KW(4851),KW(801),KW(4387),KW(4756)};
static constexpr std::string_view EKW_1491[] = {KW(4852),KW(3068),KW(649),KW(4853),KW(4780),KW(2737),KW(761),KW(4496),KW(4854),KW(4086),KW(897),KW(801),KW(4387),KW(4756)};
static constexpr std::string_view EKW_1492[] = {KW(4855),KW(3068),KW(312),KW(954),KW(4780),KW(4856),KW(761),KW(4086),KW(4387)};
static constexpr std::string_view EKW_1493[] = {KW(4857),KW(649),KW(3068),KW(4780),KW(761),KW(4496),KW(312),KW(4086),KW(4858),KW(801),KW(4387),KW(4859)};
static constexpr std::string_view EKW_1494[] = {KW(4860),KW(4856),KW(4780),KW(2737),KW(761),KW(4496),KW(312),KW(4086),KW(4750),KW(3587),KW(4851),KW(801),KW(4387),KW(4756)};
static constexpr std::string_view EKW_1495[] = {KW(4861),KW(3068),KW(4387),KW(954),KW(955),KW(4771),KW(3044),KW(4780),KW(4086),KW(381),KW(155),KW(373)};
static constexpr std::string_view EKW_1496[] = {KW(4862),KW(3068),KW(954),KW(3044),KW(4780),KW(761),KW(4496),KW(4086),KW(4387)};
static constexpr std::string_view EKW_1497[] = {KW(4863),KW(3068),KW(954),KW(2410),KW(4780),KW(314),KW(4086),KW(381),KW(155),KW(4387)};
static constexpr std::string_view EKW_1498[] = {KW(4864),KW(3068),KW(954),KW(2410),KW(4780),KW(761),KW(4496),KW(4086),KW(4387)};
static constexpr std::string_view EKW_1499[] = {KW(4865),KW(4853),KW(3068),KW(2737),KW(4496),KW(801),KW(4756)};
static constexpr std::string_view EKW_1500[] = {KW(4866),KW(3068),KW(761),KW(4867),KW(3944),KW(801)};
static constexpr std::string_view EKW_1501[] = {KW(4868),KW(3068),KW(761),KW(815),KW(4867),KW(801)};
static constexpr std::string_view EKW_1502[] = {KW(4869),KW(3068),KW(801)};
static constexpr std::string_view EKW_1503[] = {KW(4152),KW(3068),KW(4130),KW(4151),KW(4155),KW(4870),KW(4871),KW(3789),KW(1535),KW(1070),KW(3027),KW(4872),KW(4109),KW(801)};
static constexpr std::string_view EKW_1504[] = {KW(4873),KW(449),KW(4874),KW(2867),KW(440),KW(3452),KW(4875),KW(3977),KW(801)};
static constexpr std::string_view EKW_1505[] = {KW(4876),KW(449),KW(2151),KW(3452),KW(885),KW(3982),KW(801)};
static constexpr std::string_view EKW_1506[] = {KW(4877),KW(3068),KW(4878),KW(952),KW(2832),KW(4879),KW(722),KW(4880),KW(4881),KW(2737),KW(2289),KW(4882),KW(2916),KW(4883),KW(3174),KW(4884),KW(1081),KW(4080)};
static constexpr std::string_view EKW_1507[] = {KW(4039),KW(722),KW(2832),KW(4885),KW(3174)};
static constexpr std::string_view EKW_1508[] = {KW(4886),KW(4670),KW(952),KW(2289),KW(2916),KW(97),KW(4883),KW(243),KW(4080)};
static constexpr std::string_view EKW_1509[] = {KW(4887),KW(241),KW(4670),KW(178),KW(176),KW(2289),KW(2916),KW(4080)};
static constexpr std::string_view EKW_1510[] = {KW(4888),KW(1148),KW(1155),KW(1182),KW(1135),KW(801),KW(4889)};
static constexpr std::string_view EKW_1511[] = {KW(4890),KW(1143),KW(1134),KW(1154),KW(4891),KW(801)};
static constexpr std::string_view EKW_1512[] = {KW(4892),KW(4893),KW(4894),KW(1147),KW(1851),KW(1144),KW(562),KW(165),KW(384)};
static constexpr std::string_view EKW_1513[] = {KW(4895),KW(4149),KW(4146),KW(4896),KW(734),KW(4897),KW(801),KW(1215)};
static constexpr std::string_view EKW_1514[] = {KW(4898),KW(4149),KW(4146),KW(4899),KW(4900),KW(3982),KW(734),KW(801),KW(4901)};
static constexpr std::string_view EKW_1515[] = {KW(4902),KW(4149),KW(4146),KW(4903),KW(4904),KW(734),KW(801),KW(3530)};
static constexpr std::string_view EKW_1516[] = {KW(4905),KW(4906),KW(4149),KW(4146),KW(734),KW(801)};
static constexpr std::string_view EKW_1517[] = {KW(4907),KW(4149),KW(4908)};
static constexpr std::string_view EKW_1518[] = {KW(4909),KW(4910),KW(2303),KW(4911),KW(165),KW(4912),KW(4913)};
static constexpr std::string_view EKW_1519[] = {KW(4914),KW(4688),KW(173),KW(4915),KW(737),KW(381)};
static constexpr std::string_view EKW_1520[] = {KW(4916),KW(4917),KW(737),KW(173),KW(4918),KW(381)};
static constexpr std::string_view EKW_1521[] = {KW(4919),KW(1262),KW(409),KW(761),KW(736),KW(737),KW(381)};
static constexpr std::string_view EKW_1522[] = {KW(4920),KW(4921),KW(3442),KW(461),KW(409),KW(604),KW(736),KW(117),KW(737)};
static constexpr std::string_view EKW_1523[] = {KW(4922),KW(173),KW(737),KW(3442),KW(103),KW(3172),KW(604),KW(736),KW(117)};
static constexpr std::string_view EKW_1524[] = {KW(4923),KW(4924),KW(603),KW(173),KW(737),KW(103),KW(3172),KW(808),KW(381),KW(801)};
static constexpr std::string_view EKW_1525[] = {KW(4925),KW(1385),KW(4926),KW(435),KW(1160),KW(4927),KW(4928),KW(737),KW(867)};
static constexpr std::string_view EKW_1526[] = {KW(4929),KW(164),KW(2925),KW(163),KW(2842),KW(2924)};
static constexpr std::string_view EKW_1527[] = {KW(4930),KW(164),KW(2925),KW(4246),KW(4250),KW(2008)};
static constexpr std::string_view EKW_1528[] = {KW(4931),KW(1107),KW(1269),KW(4932),KW(4933),KW(4934),KW(1921),KW(347),KW(348),KW(3680),KW(2185),KW(4935)};
static constexpr std::string_view EKW_1529[] = {KW(4936),KW(4780),KW(2853),KW(4397),KW(4395),KW(761),KW(364),KW(3792),KW(4937),KW(4913),KW(4938)};
static constexpr std::string_view EKW_1530[] = {KW(4939),KW(4940),KW(3820),KW(2968),KW(4941),KW(4942),KW(4943)};
static constexpr std::string_view EKW_1531[] = {KW(4944),KW(2810),KW(4945),KW(3185),KW(4946),KW(1760),KW(2813)};
static constexpr std::string_view EKW_1532[] = {KW(4947),KW(1411),KW(4698),KW(4243),KW(4948)};
static constexpr std::string_view EKW_1533[] = {KW(4949),KW(4950),KW(4441),KW(4951),KW(364),KW(2355),KW(735),KW(4952),KW(2813),KW(1183)};
static constexpr std::string_view EKW_1534[] = {KW(4953),KW(815),KW(1014),KW(4954),KW(734),KW(153),KW(735),KW(4955)};
static constexpr std::string_view EKW_1535[] = {KW(4956),KW(4279),KW(208),KW(976),KW(4316),KW(4315),KW(4957),KW(4958),KW(364),KW(734),KW(801),KW(119),KW(4959)};
static constexpr std::string_view EKW_1536[] = {KW(4960),KW(208),KW(976),KW(4961),KW(4962),KW(4316),KW(4315),KW(211),KW(4958),KW(4963),KW(4964),KW(735)};
static constexpr std::string_view EKW_1537[] = {KW(4965),KW(208),KW(4966),KW(4957),KW(211),KW(4958),KW(734)};
static constexpr std::string_view EKW_1538[] = {KW(4967),KW(242),KW(4968),KW(3530),KW(4896),KW(381),KW(4969),KW(4897),KW(1215)};
static constexpr std::string_view EKW_1539[] = {KW(4970),KW(1215),KW(4279),KW(242),KW(4971),KW(4972),KW(3944),KW(4973)};
static constexpr std::string_view EKW_1540[] = {KW(4974),KW(4928),KW(1385),KW(4759),KW(4927),KW(4975),KW(4976)};
static constexpr std::string_view EKW_1541[] = {KW(4977),KW(4158),KW(4978),KW(4975),KW(4976),KW(4979)};
static constexpr std::string_view EKW_1542[] = {KW(4980),KW(4280),KW(4281),KW(4282),KW(1335),KW(4358),KW(624),KW(4981),KW(4982)};
static constexpr std::string_view EKW_1543[] = {KW(4983),KW(812),KW(709),KW(4279)};
static constexpr std::string_view EKW_1544[] = {KW(4984),KW(4670),KW(4759),KW(4985),KW(761),KW(2391)};
static constexpr std::string_view EKW_1545[] = {KW(709),KW(1406),KW(4279),KW(975),KW(935),KW(3542)};
static constexpr std::string_view EKW_1546[] = {KW(4986),KW(4987),KW(4223),KW(815),KW(1294),KW(1422),KW(4988),KW(165)};
static constexpr std::string_view EKW_1547[] = {KW(4989),KW(4990),KW(815),KW(4991),KW(165)};
static constexpr std::string_view EKW_1548[] = {KW(4992),KW(4993),KW(770),KW(1294),KW(1422),KW(165),KW(4994)};
static constexpr std::string_view EKW_1549[] = {KW(4995)};
static constexpr std::string_view EKW_1550[] = {KW(4996),KW(801),KW(3068),KW(4997),KW(4998),KW(4999),KW(4415),KW(4843),KW(5000),KW(5001),KW(165)};
static constexpr std::string_view EKW_1551[] = {KW(4996),KW(812),KW(5002),KW(5003)};
static constexpr std::string_view EKW_1552[] = {KW(5004),KW(5005),KW(791),KW(3068),KW(5006),KW(5007),KW(5008)};
static constexpr std::string_view EKW_1553[] = {KW(5009),KW(3068),KW(791),KW(3295),KW(154),KW(5008)};
static constexpr std::string_view EKW_1554[] = {KW(5010),KW(791),KW(3307),KW(5011),KW(3068),KW(724),KW(5008)};
static constexpr std::string_view EKW_1555[] = {KW(5012),KW(110),KW(791),KW(5011),KW(3068),KW(95),KW(5008)};
static constexpr std::string_view EKW_1556[] = {KW(5013),KW(3332),KW(791),KW(3068),KW(5014),KW(5008)};
static constexpr std::string_view EKW_1557[] = {KW(5015),KW(3345),KW(791),KW(3068),KW(5011),KW(880),KW(5008)};
static constexpr std::string_view EKW_1558[] = {KW(5016),KW(3358),KW(791),KW(3068),KW(5017),KW(5008)};
static constexpr std::string_view EKW_1559[] = {KW(5018),KW(3371),KW(791),KW(3068),KW(5011),KW(5019),KW(5008)};
static constexpr std::string_view EKW_1560[] = {KW(5020),KW(3384),KW(3068),KW(791),KW(3719),KW(5008)};
static constexpr std::string_view EKW_1561[] = {KW(5021),KW(3068),KW(791),KW(3397),KW(4824),KW(5008)};
static constexpr std::string_view EKW_1562[] = {KW(5022),KW(791),KW(3410),KW(3068),KW(3656),KW(4843)};
static constexpr std::string_view EKW_1563[] = {KW(5023),KW(4990),KW(839),KW(5024),KW(5025),KW(3068),KW(5026),KW(5027),KW(4867),KW(801)};
static constexpr std::string_view EKW_1564[] = {KW(5028),KW(3068),KW(5024),KW(5029),KW(4990),KW(5026),KW(4867),KW(155),KW(801)};
static constexpr std::string_view EKW_1565[] = {KW(5030),KW(791),KW(3068),KW(5031),KW(3295),KW(3307),KW(110),KW(3332),KW(4867),KW(5032),KW(801)};
static constexpr std::string_view EKW_1566[] = {KW(5033),KW(3068),KW(1375),KW(697),KW(5034),KW(5035),KW(5036),KW(5037),KW(4867),KW(801),KW(5038)};
static constexpr std::string_view EKW_1567[] = {KW(5039),KW(3068),KW(4990),KW(5040),KW(4867),KW(801)};
static constexpr std::string_view EKW_1568[] = {KW(5041),KW(5042),KW(4990),KW(4290),KW(1573),KW(5027),KW(5043),KW(4972),KW(4973),KW(4122)};
static constexpr std::string_view EKW_1569[] = {KW(5044),KW(5042),KW(4990),KW(1573),KW(4972),KW(4973),KW(4122)};
static constexpr std::string_view EKW_1570[] = {KW(5045),KW(5042),KW(4990),KW(4290),KW(1573),KW(5027),KW(5043),KW(4972),KW(4973),KW(4122)};
static constexpr std::string_view EKW_1571[] = {KW(5046),KW(4990),KW(839),KW(5042),KW(5047),KW(165),KW(4973)};
static constexpr std::string_view EKW_1572[] = {KW(5048),KW(839),KW(3068),KW(165),KW(3944),KW(4973)};
static constexpr std::string_view EKW_1573[] = {KW(5049),KW(3068),KW(839),KW(165),KW(4973)};
static constexpr std::string_view EKW_1574[] = {KW(1225),KW(3068),KW(4990),KW(4290),KW(851),KW(849),KW(5029),KW(5050),KW(5051)};
static constexpr std::string_view EKW_1575[] = {KW(5052),KW(727),KW(839),KW(5053),KW(5054),KW(165),KW(4973)};
static constexpr std::string_view EKW_1576[] = {KW(5055),KW(4990),KW(5056),KW(4290),KW(5027),KW(815),KW(5043),KW(3067)};
static constexpr std::string_view EKW_1577[] = {KW(5057),KW(3068),KW(839),KW(5058),KW(2992),KW(165),KW(4973)};
static constexpr std::string_view EKW_1578[] = {KW(5059),KW(3068),KW(839),KW(4759),KW(4177),KW(5060),KW(935),KW(242),KW(165),KW(4973)};
static constexpr std::string_view EKW_1579[] = {KW(5061),KW(4990),KW(5042),KW(4290),KW(1573),KW(5027),KW(5043),KW(4972),KW(5062),KW(4973),KW(4122)};
static constexpr std::string_view EKW_1580[] = {KW(5063),KW(935),KW(976),KW(211),KW(3068),KW(915),KW(165),KW(3944),KW(4973)};
static constexpr std::string_view EKW_1581[] = {KW(5064),KW(3091),KW(3068),KW(4990),KW(4290),KW(5027),KW(5043),KW(4972),KW(5065),KW(165),KW(4973)};
static constexpr std::string_view EKW_1582[] = {KW(5066),KW(1231),KW(5042),KW(839),KW(2921),KW(1425),KW(5067),KW(165),KW(3174),KW(4973)};
static constexpr std::string_view EKW_1583[] = {KW(5068),KW(3068),KW(730),KW(885),KW(4688),KW(5069),KW(735),KW(165),KW(4973),KW(5070)};
static constexpr std::string_view EKW_1584[] = {KW(5071),KW(839),KW(4670),KW(4973),KW(5072)};
static constexpr std::string_view EKW_1585[] = {KW(5073),KW(3068),KW(4371),KW(5074),KW(1448),KW(5075),KW(5076),KW(5077),KW(165),KW(4973),KW(5078),KW(5079)};
static constexpr std::string_view EKW_1586[] = {KW(5080),KW(1448),KW(3068),KW(5074),KW(5081),KW(5077),KW(1429),KW(5082),KW(165),KW(4973),KW(5083),KW(5084)};
static constexpr std::string_view EKW_1587[] = {KW(5085),KW(1486),KW(5086),KW(1403),KW(1448),KW(4670),KW(3780),KW(5087),KW(5088),KW(5089),KW(5077),KW(5090),KW(165),KW(4973),KW(5091),KW(5092),KW(5093)};
static constexpr std::string_view EKW_1588[] = {KW(5094),KW(4670),KW(1486),KW(5095),KW(3780),KW(5081),KW(5088),KW(5096),KW(5089),KW(5077),KW(5097),KW(165),KW(4973),KW(5098),KW(5092),KW(5099)};
static constexpr std::string_view EKW_1589[] = {KW(5100),KW(1486),KW(955),KW(4279),KW(3780),KW(5088),KW(193),KW(5089),KW(5077),KW(165),KW(4973),KW(5101),KW(5092),KW(5102)};
static constexpr std::string_view EKW_1590[] = {KW(5103),KW(1486),KW(3780),KW(5104),KW(5105),KW(815),KW(5106),KW(5107),KW(4842),KW(5089),KW(5077),KW(165),KW(5108)};
static constexpr std::string_view EKW_1591[] = {KW(5109),KW(2529),KW(4906),KW(1486),KW(3780),KW(5110),KW(5111),KW(5088),KW(5089),KW(5077),KW(4244),KW(165),KW(4973),KW(5112),KW(5092),KW(5113)};
static constexpr std::string_view EKW_1592[] = {KW(5114),KW(5115),KW(1486),KW(3780),KW(1448),KW(4670),KW(5081),KW(5088),KW(5089),KW(5116),KW(5077),KW(5117),KW(165),KW(4973),KW(5118),KW(5092),KW(5119)};
static constexpr std::string_view EKW_1593[] = {KW(5120),KW(3780),KW(1448),KW(1486),KW(680),KW(3257),KW(4703),KW(5042),KW(5088),KW(4698),KW(5089),KW(5077),KW(5121),KW(165),KW(4973),KW(5122),KW(5092),KW(5123)};
static constexpr std::string_view EKW_1594[] = {KW(5124),KW(208),KW(935),KW(1486),KW(3780),KW(976),KW(211),KW(5125),KW(977),KW(4842),KW(5089),KW(5077),KW(165),KW(5126)};
static constexpr std::string_view EKW_1595[] = {KW(5127),KW(1486),KW(1448),KW(3780),KW(4670),KW(5128),KW(5088),KW(5129),KW(5089),KW(5077),KW(677),KW(5130),KW(165),KW(4973),KW(5131),KW(5092),KW(5132)};
static constexpr std::string_view EKW_1596[] = {KW(5133),KW(1448),KW(1486),KW(5134),KW(3780),KW(5042),KW(1037),KW(5088),KW(5135),KW(5089),KW(5077),KW(165),KW(4973),KW(1054),KW(5136),KW(5092),KW(5137)};
static constexpr std::string_view EKW_1597[] = {KW(5138),KW(3780),KW(1448),KW(1486),KW(2769),KW(3013),KW(3068),KW(5139),KW(5140),KW(5088),KW(5089),KW(5077),KW(165),KW(4973),KW(5141),KW(5092),KW(5142)};
static constexpr std::string_view EKW_1598[] = {KW(5143),KW(1486),KW(3780),KW(1448),KW(5144),KW(4842),KW(5145),KW(5146),KW(5089),KW(5077),KW(165),KW(5147)};
static constexpr std::string_view EKW_1599[] = {KW(5148),KW(4407),KW(1486),KW(5149),KW(3780),KW(5144),KW(4842),KW(5089),KW(5077),KW(165),KW(5150)};
static constexpr std::string_view EKW_1600[] = {KW(5151),KW(1448),KW(5152),KW(4670),KW(5153),KW(5154),KW(1486),KW(5088),KW(5089),KW(5077),KW(5155),KW(165),KW(4973),KW(5156),KW(5092),KW(551),KW(5157)};
static constexpr std::string_view EKW_1601[] = {KW(5158),KW(799),KW(1486),KW(1448),KW(5042),KW(3780),KW(5159),KW(5088),KW(5160),KW(5089),KW(5077),KW(165),KW(4973),KW(5161),KW(5092),KW(5162),KW(5163),KW(5164)};
static constexpr std::string_view EKW_1602[] = {KW(5165),KW(4759),KW(3163),KW(603),KW(5166),KW(153)};
static constexpr std::string_view EKW_1603[] = {KW(5167),KW(1014),KW(5166),KW(153)};
static constexpr std::string_view EKW_1604[] = {KW(5168),KW(1014),KW(5166),KW(153)};
static constexpr std::string_view EKW_1605[] = {KW(5169),KW(1014),KW(5166),KW(153)};
static constexpr std::string_view EKW_1606[] = {KW(5170),KW(4759),KW(4177),KW(4126),KW(5166),KW(153)};
static constexpr std::string_view EKW_1607[] = {KW(5171),KW(1014),KW(5166),KW(153)};
static constexpr std::string_view EKW_1608[] = {KW(5172),KW(1014),KW(5166),KW(153)};
static constexpr std::string_view EKW_1609[] = {KW(5173),KW(4759),KW(4126),KW(1014),KW(5166),KW(3462)};
static constexpr std::string_view EKW_1610[] = {KW(5174),KW(4759),KW(1014),KW(5166),KW(3462)};
static constexpr std::string_view EKW_1611[] = {KW(5175),KW(3766),KW(5166),KW(153)};
static constexpr std::string_view EKW_1612[] = {KW(5176),KW(5166),KW(153)};
static constexpr std::string_view EKW_1613[] = {KW(5177),KW(3766),KW(5166),KW(153)};
static constexpr std::string_view EKW_1614[] = {KW(5178),KW(5166),KW(153)};
static constexpr std::string_view EKW_1615[] = {KW(5179),KW(5166),KW(153)};
static constexpr std::string_view EKW_1616[] = {KW(5180),KW(5166),KW(153)};
static constexpr std::string_view EKW_1617[] = {KW(5181),KW(5166),KW(153)};
static constexpr std::string_view EKW_1618[] = {KW(5182),KW(4759),KW(4177),KW(4126),KW(5166)};
static constexpr std::string_view EKW_1619[] = {KW(5183),KW(4759),KW(4177),KW(2890),KW(4126),KW(5166)};
static constexpr std::string_view EKW_1620[] = {KW(5184),KW(4759),KW(4126),KW(4177),KW(5166)};
static constexpr std::string_view EKW_1621[] = {KW(5185),KW(4759),KW(2890),KW(4177),KW(5166)};
static constexpr std::string_view EKW_1622[] = {KW(5186),KW(4177),KW(4759),KW(4126),KW(5166)};
static constexpr std::string_view EKW_1623[] = {KW(5187),KW(4759),KW(2890),KW(4177),KW(4126),KW(5166)};
static constexpr std::string_view EKW_1624[] = {KW(5188),KW(4759),KW(4177),KW(5166)};
static constexpr std::string_view EKW_1625[] = {KW(5189),KW(4759),KW(4177),KW(5166)};
static constexpr std::string_view EKW_1626[] = {KW(5190),KW(4759),KW(3969),KW(3963),KW(5166)};
static constexpr std::string_view EKW_1627[] = {KW(5191),KW(4759),KW(3969),KW(3963),KW(5166)};
static constexpr std::string_view EKW_1628[] = {KW(5192),KW(4759),KW(3969),KW(3963),KW(5166)};
static constexpr std::string_view EKW_1629[] = {KW(5193),KW(4759),KW(3969),KW(3963),KW(5166)};
static constexpr std::string_view EKW_1630[] = {KW(5194),KW(4759),KW(954),KW(373),KW(3044),KW(5166),KW(4086),KW(155)};
static constexpr std::string_view EKW_1631[] = {KW(5195),KW(4759),KW(954),KW(2410),KW(5166),KW(4086),KW(155)};
static constexpr std::string_view EKW_1632[] = {KW(5196),KW(3969),KW(386),KW(3963),KW(5197),KW(5198),KW(618),KW(5199),KW(2294),KW(5166),KW(5200),KW(21),KW(4759)};
static constexpr std::string_view EKW_1633[] = {KW(5201),KW(4123),KW(1176),KW(1375),KW(815),KW(5166)};
static constexpr std::string_view EKW_1634[] = {KW(5202),KW(4759),KW(4123),KW(5166),KW(117)};
static constexpr std::string_view EKW_1635[] = {KW(5203),KW(4759),KW(4123),KW(3799),KW(5166)};
static constexpr std::string_view EKW_1636[] = {KW(5204),KW(3615),KW(5205),KW(1678),KW(5206),KW(5207),KW(5208),KW(1135),KW(5209),KW(5210),KW(1679)};
static constexpr std::string_view EKW_1637[] = {KW(5211),KW(735),KW(5210),KW(2872),KW(5212),KW(3996),KW(381),KW(3575)};
static constexpr std::string_view EKW_1638[] = {KW(5213),KW(1448),KW(2844),KW(2845),KW(4677),KW(1535),KW(426),KW(403),KW(3575),KW(724)};
static constexpr std::string_view EKW_1639[] = {KW(5214),KW(611),KW(5215)};
static constexpr std::string_view EKW_1640[] = {KW(5216),KW(4265),KW(5217),KW(5218),KW(5219),KW(5220),KW(615),KW(5215)};
static constexpr std::string_view EKW_1641[] = {KW(5221),KW(3575),KW(3499),KW(562),KW(1857),KW(1851),KW(5222),KW(5223),KW(1850),KW(5224)};
static constexpr std::string_view EKW_1642[] = {KW(5225),KW(4893),KW(3575),KW(562),KW(4894),KW(386),KW(1851),KW(2151),KW(713),KW(5226),KW(119)};
static constexpr std::string_view EKW_1643[] = {KW(5227),KW(598),KW(5228),KW(3575),KW(3562),KW(5229),KW(5230),KW(5231),KW(4251)};
static constexpr std::string_view EKW_1644[] = {KW(5232)};
static constexpr std::string_view EKW_1645[] = {KW(5233),KW(5234),KW(3575),KW(2844),KW(2845),KW(3562),KW(5235),KW(5236)};
static constexpr std::string_view EKW_1646[] = {KW(5237),KW(5238),KW(1477),KW(5239),KW(3575),KW(2844),KW(2845),KW(3562),KW(5240),KW(5241),KW(5242)};
static constexpr std::string_view EKW_1647[] = {KW(5243),KW(5244),KW(3575),KW(2844),KW(2845),KW(3562),KW(5245),KW(5246)};
static constexpr std::string_view EKW_1648[] = {KW(5247),KW(5248),KW(5249),KW(3575),KW(2844),KW(2845),KW(3562),KW(5250)};
static constexpr std::string_view EKW_1649[] = {KW(5251),KW(5252),KW(3575),KW(2844),KW(2845),KW(3562),KW(5253),KW(5254)};
static constexpr std::string_view EKW_1650[] = {KW(5255),KW(5256),KW(3575),KW(2844),KW(2845),KW(3562),KW(5257),KW(5258)};
static constexpr std::string_view EKW_1651[] = {KW(5259),KW(850),KW(3575),KW(2844),KW(2845),KW(3562),KW(5260),KW(5261)};
static constexpr std::string_view EKW_1652[] = {KW(5262),KW(5263),KW(3575),KW(2844),KW(2845),KW(3562),KW(5264),KW(5265)};
static constexpr std::string_view EKW_1653[] = {KW(5266),KW(5267),KW(3575),KW(2844),KW(2845),KW(3562),KW(5268),KW(5269)};
static constexpr std::string_view EKW_1654[] = {KW(5270),KW(5271),KW(3575),KW(2844),KW(2845),KW(3562),KW(5272),KW(5273)};
static constexpr std::string_view EKW_1655[] = {KW(5274),KW(2966),KW(5275),KW(3575),KW(2844),KW(2845),KW(3562),KW(5276),KW(5277)};
static constexpr std::string_view EKW_1656[] = {KW(5278),KW(5279),KW(3575),KW(2844),KW(2845),KW(3562),KW(5280),KW(5281)};
static constexpr std::string_view EKW_1657[] = {KW(5282),KW(5283),KW(3575),KW(2844),KW(2845),KW(3562),KW(2110),KW(5284),KW(5285),KW(5286),KW(5287)};
static constexpr std::string_view EKW_1658[] = {KW(5288),KW(511),KW(3575),KW(2844),KW(2845),KW(3562),KW(5289),KW(5290)};
static constexpr std::string_view EKW_1659[] = {KW(5291),KW(5292),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5294)};
static constexpr std::string_view EKW_1660[] = {KW(5295),KW(5296),KW(3575),KW(2844),KW(2845),KW(3562),KW(5297),KW(5298)};
static constexpr std::string_view EKW_1661[] = {KW(5299),KW(5300),KW(5301),KW(3575),KW(2844),KW(2845),KW(3562),KW(5302)};
static constexpr std::string_view EKW_1662[] = {KW(5303),KW(5304),KW(3575),KW(2844),KW(2845),KW(3562),KW(5305),KW(5306),KW(5307)};
static constexpr std::string_view EKW_1663[] = {KW(5308),KW(5309),KW(3575),KW(2844),KW(2845),KW(3562),KW(5310),KW(5311)};
static constexpr std::string_view EKW_1664[] = {KW(5312),KW(5313),KW(3575),KW(2844),KW(2845),KW(3562),KW(5314),KW(5315)};
static constexpr std::string_view EKW_1665[] = {KW(5316),KW(5317),KW(5318),KW(3575),KW(2844),KW(2845),KW(3562),KW(5319),KW(5320)};
static constexpr std::string_view EKW_1666[] = {KW(5321),KW(5322),KW(3575),KW(2844),KW(2845),KW(3562),KW(5323),KW(5324)};
static constexpr std::string_view EKW_1667[] = {KW(5325),KW(5326),KW(3575),KW(2844),KW(2845),KW(3562),KW(5327),KW(5328),KW(5329)};
static constexpr std::string_view EKW_1668[] = {KW(5330),KW(5331),KW(3575),KW(2844),KW(2845),KW(3562),KW(5332),KW(5333)};
static constexpr std::string_view EKW_1669[] = {KW(5334),KW(5335),KW(3575),KW(2844),KW(2845),KW(3562),KW(5336),KW(5337)};
static constexpr std::string_view EKW_1670[] = {KW(5338),KW(1537),KW(5339),KW(3575),KW(2844),KW(2845),KW(3562),KW(5340),KW(5341)};
static constexpr std::string_view EKW_1671[] = {KW(5342),KW(5343),KW(3575),KW(2844),KW(2845),KW(3562),KW(5344),KW(5345)};
static constexpr std::string_view EKW_1672[] = {KW(5346),KW(5347),KW(5348),KW(3575),KW(2844),KW(2845),KW(3562),KW(5349),KW(5350)};
static constexpr std::string_view EKW_1673[] = {KW(5351),KW(5352),KW(3575),KW(2844),KW(2845),KW(3562),KW(5353),KW(5354)};
static constexpr std::string_view EKW_1674[] = {KW(5355),KW(5356),KW(3575),KW(2844),KW(2845),KW(3562),KW(5357),KW(5358),KW(5359),KW(5360)};
static constexpr std::string_view EKW_1675[] = {KW(5361),KW(5362),KW(3575),KW(2844),KW(2845),KW(3562),KW(5363),KW(5364),KW(5365),KW(4867)};
static constexpr std::string_view EKW_1676[] = {KW(5366),KW(5367),KW(3575),KW(2844),KW(2845),KW(3562),KW(5368),KW(5369)};
static constexpr std::string_view EKW_1677[] = {KW(5370),KW(5371),KW(3575),KW(2844),KW(2845),KW(3562),KW(5372),KW(5373)};
static constexpr std::string_view EKW_1678[] = {KW(5374),KW(5375)};
static constexpr std::string_view EKW_1679[] = {KW(5376),KW(5377),KW(3575),KW(2844),KW(2845),KW(3562),KW(5378),KW(5379)};
static constexpr std::string_view EKW_1680[] = {KW(5380),KW(5381),KW(3575),KW(2844),KW(2845),KW(3562),KW(5382),KW(5383)};
static constexpr std::string_view EKW_1681[] = {KW(5384),KW(5385),KW(3575),KW(2844),KW(2845),KW(3562),KW(5386),KW(5387)};
static constexpr std::string_view EKW_1682[] = {KW(5388),KW(2363),KW(3575),KW(2844),KW(2845),KW(3562),KW(1990),KW(2364)};
static constexpr std::string_view EKW_1683[] = {KW(5389),KW(5390),KW(5391),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5392),KW(5393)};
static constexpr std::string_view EKW_1684[] = {KW(5394),KW(5395),KW(5396),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(5398),KW(5399)};
static constexpr std::string_view EKW_1685[] = {KW(5400),KW(5401),KW(1985),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(5402)};
static constexpr std::string_view EKW_1686[] = {KW(5403),KW(5395),KW(3575),KW(2844),KW(2845),KW(3562),KW(5404),KW(5397)};
static constexpr std::string_view EKW_1687[] = {KW(5405),KW(5406),KW(3575),KW(2844),KW(2845),KW(3562),KW(5407),KW(403),KW(381),KW(2520)};
static constexpr std::string_view EKW_1688[] = {KW(5408),KW(5409),KW(5410),KW(3575),KW(2844),KW(2845),KW(3562),KW(5411),KW(5412),KW(5413),KW(5414)};
static constexpr std::string_view EKW_1689[] = {KW(5415),KW(1309),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5416),KW(5393),KW(5417)};
static constexpr std::string_view EKW_1690[] = {KW(5418),KW(3575),KW(2844),KW(2845),KW(3562),KW(5419),KW(5420)};
static constexpr std::string_view EKW_1691[] = {KW(5421),KW(5422),KW(3575),KW(2844),KW(2845),KW(3562),KW(5423),KW(5424)};
static constexpr std::string_view EKW_1692[] = {KW(5425),KW(5426),KW(1486),KW(5427),KW(3575),KW(2845),KW(2844),KW(3562),KW(5428),KW(5429),KW(5024),KW(5430),KW(801)};
static constexpr std::string_view EKW_1693[] = {KW(5431),KW(5432),KW(3575),KW(2844),KW(2845),KW(3562),KW(5433),KW(5434)};
static constexpr std::string_view EKW_1694[] = {KW(5435)};
static constexpr std::string_view EKW_1695[] = {KW(5436),KW(5437),KW(3575),KW(3562)};
static constexpr std::string_view EKW_1696[] = {KW(5438),KW(5439),KW(5440),KW(3575),KW(2844),KW(2845),KW(3562),KW(5441),KW(5442)};
static constexpr std::string_view EKW_1697[] = {KW(5443),KW(5444),KW(3575),KW(2844),KW(2845),KW(3562),KW(5445),KW(5446)};
static constexpr std::string_view EKW_1698[] = {KW(5447),KW(5448),KW(5449),KW(3575),KW(2844),KW(2845),KW(3562),KW(5450),KW(5451)};
static constexpr std::string_view EKW_1699[] = {KW(5452),KW(5453),KW(3575),KW(2844),KW(2845),KW(3562),KW(5454),KW(5455),KW(5456)};
static constexpr std::string_view EKW_1700[] = {KW(5457),KW(1544),KW(5393),KW(3575),KW(2844),KW(2845),KW(3562),KW(5458)};
static constexpr std::string_view EKW_1701[] = {KW(5459),KW(5460),KW(3575),KW(2844),KW(2845),KW(3562),KW(5461),KW(5462)};
static constexpr std::string_view EKW_1702[] = {KW(5463),KW(5464),KW(3575),KW(2844),KW(2845),KW(3562),KW(5465),KW(5466),KW(5397)};
static constexpr std::string_view EKW_1703[] = {KW(5467),KW(5468),KW(2844),KW(3575),KW(2845),KW(3562),KW(2500),KW(5469),KW(5470),KW(5429),KW(5024),KW(5430),KW(801)};
static constexpr std::string_view EKW_1704[] = {KW(5471)};
static constexpr std::string_view EKW_1705[] = {KW(5472),KW(5473),KW(3575),KW(2844),KW(2845),KW(3562),KW(5474),KW(5475)};
static constexpr std::string_view EKW_1706[] = {KW(5476),KW(5477),KW(3575),KW(2844),KW(2845),KW(3562),KW(5478),KW(5479)};
static constexpr std::string_view EKW_1707[] = {KW(5480),KW(5481),KW(3575),KW(2844),KW(2845),KW(3562),KW(5482)};
static constexpr std::string_view EKW_1708[] = {KW(5483),KW(5484),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(5485),KW(5486),KW(5487)};
static constexpr std::string_view EKW_1709[] = {KW(5488),KW(5489),KW(3575),KW(2844),KW(2845),KW(3562),KW(5490),KW(5491)};
static constexpr std::string_view EKW_1710[] = {KW(5492)};
static constexpr std::string_view EKW_1711[] = {KW(5493),KW(5494),KW(3575),KW(2844),KW(2845),KW(3562),KW(5495),KW(5496)};
static constexpr std::string_view EKW_1712[] = {KW(5497),KW(5498),KW(3575),KW(2844),KW(2845),KW(3562),KW(5499),KW(5500)};
static constexpr std::string_view EKW_1713[] = {KW(5501),KW(5502),KW(3575),KW(2844),KW(2845),KW(3562),KW(5503),KW(5504)};
static constexpr std::string_view EKW_1714[] = {KW(5505),KW(5506),KW(5507),KW(3575),KW(2844),KW(2845),KW(3562),KW(5508),KW(5509),KW(4748)};
static constexpr std::string_view EKW_1715[] = {KW(5510),KW(5511),KW(3575),KW(2844),KW(2845),KW(3562),KW(5512),KW(5513)};
static constexpr std::string_view EKW_1716[] = {KW(5514),KW(5515),KW(3575),KW(2844),KW(2845),KW(3562),KW(5516),KW(5517),KW(5429),KW(5024),KW(5518),KW(5430),KW(5519),KW(801)};
static constexpr std::string_view EKW_1717[] = {KW(5520),KW(643),KW(3575),KW(2844),KW(2845),KW(3562),KW(5521),KW(5522)};
static constexpr std::string_view EKW_1718[] = {KW(5523),KW(2919),KW(3645),KW(3575),KW(3562),KW(5524)};
static constexpr std::string_view EKW_1719[] = {KW(5525),KW(5526),KW(3575),KW(2844),KW(2845),KW(3562),KW(5527),KW(5528)};
static constexpr std::string_view EKW_1720[] = {KW(5529),KW(5530),KW(3575),KW(2844),KW(2845),KW(3562),KW(5531),KW(5532)};
static constexpr std::string_view EKW_1721[] = {KW(5533),KW(5534),KW(5293),KW(5535),KW(3575),KW(2844),KW(2845),KW(3562),KW(5536),KW(5537),KW(5538),KW(5393),KW(5539)};
static constexpr std::string_view EKW_1722[] = {KW(5540),KW(5541),KW(5542),KW(5543),KW(3575),KW(2844),KW(2845),KW(3562),KW(5544)};
static constexpr std::string_view EKW_1723[] = {KW(5545),KW(5546),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5547),KW(5393),KW(5417)};
static constexpr std::string_view EKW_1724[] = {KW(5548),KW(3562),KW(3575),KW(2844),KW(2489),KW(2486),KW(2845),KW(5549),KW(5550),KW(5429),KW(5393),KW(5024),KW(5551),KW(5430),KW(1537),KW(5341),KW(801)};
static constexpr std::string_view EKW_1725[] = {KW(5552),KW(5553),KW(3575),KW(2844),KW(2845),KW(3562),KW(5554),KW(5555)};
static constexpr std::string_view EKW_1726[] = {KW(5556),KW(5238),KW(5557),KW(2219),KW(5558),KW(5559),KW(2361),KW(3575),KW(2844),KW(2845),KW(3562),KW(1437),KW(5560),KW(5561),KW(4260),KW(5562),KW(5563),KW(5564),KW(1441),KW(5565),KW(5566)};
static constexpr std::string_view EKW_1727[] = {KW(5567),KW(5568),KW(3575),KW(2844),KW(2845),KW(3562),KW(5569),KW(5570)};
static constexpr std::string_view EKW_1728[] = {KW(5571),KW(5572),KW(3575),KW(2844),KW(2845),KW(3562),KW(5573),KW(5574)};
static constexpr std::string_view EKW_1729[] = {KW(5575),KW(2486),KW(5576),KW(3575),KW(2844),KW(2845),KW(3562),KW(5577),KW(5578)};
static constexpr std::string_view EKW_1730[] = {KW(5579),KW(5580),KW(3575),KW(2844),KW(2845),KW(3562),KW(5581)};
static constexpr std::string_view EKW_1731[] = {KW(5582),KW(5583),KW(3575),KW(2844),KW(2845),KW(3562),KW(5584),KW(5585)};
static constexpr std::string_view EKW_1732[] = {KW(5586),KW(5587),KW(3575),KW(2844),KW(2845),KW(3562),KW(5588),KW(5589)};
static constexpr std::string_view EKW_1733[] = {KW(5590),KW(5591),KW(3575),KW(2844),KW(2845),KW(3562),KW(5592),KW(5593)};
static constexpr std::string_view EKW_1734[] = {KW(5594),KW(5595),KW(3575),KW(2844),KW(2845),KW(3562),KW(5596),KW(5597)};
static constexpr std::string_view EKW_1735[] = {KW(5598),KW(5599),KW(3575),KW(2844),KW(2845),KW(3562),KW(5600),KW(5578)};
static constexpr std::string_view EKW_1736[] = {KW(5601),KW(5602),KW(3575),KW(2844),KW(2845),KW(3562),KW(5603),KW(5604)};
static constexpr std::string_view EKW_1737[] = {KW(5605),KW(5606),KW(5599),KW(3575),KW(2844),KW(2845),KW(3562),KW(5607),KW(5608),KW(5578)};
static constexpr std::string_view EKW_1738[] = {KW(5609),KW(5610),KW(3575),KW(2844),KW(2845),KW(3562),KW(5611),KW(3791)};
static constexpr std::string_view EKW_1739[] = {KW(5612),KW(4741),KW(5573),KW(2557),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5613),KW(5393)};
static constexpr std::string_view EKW_1740[] = {KW(5614),KW(5615),KW(3575),KW(2844),KW(2845),KW(3562),KW(5616),KW(5617)};
static constexpr std::string_view EKW_1741[] = {KW(5618),KW(5619),KW(3575),KW(2844),KW(2845),KW(3562),KW(5620),KW(5621),KW(5622)};
static constexpr std::string_view EKW_1742[] = {KW(5623),KW(5624),KW(5625),KW(3575),KW(2844),KW(2845),KW(3562),KW(5626)};
static constexpr std::string_view EKW_1743[] = {KW(5627),KW(5628),KW(3575),KW(2844),KW(2845),KW(3562),KW(5629),KW(5630)};
static constexpr std::string_view EKW_1744[] = {KW(5631),KW(5632),KW(5633),KW(3575),KW(2844),KW(2845),KW(3562),KW(5634)};
static constexpr std::string_view EKW_1745[] = {KW(5635)};
static constexpr std::string_view EKW_1746[] = {KW(5636),KW(5637),KW(3575),KW(2844),KW(2845),KW(3562),KW(5638),KW(5639)};
static constexpr std::string_view EKW_1747[] = {KW(5640),KW(5641),KW(3575),KW(2844),KW(2845),KW(3562),KW(5642),KW(5643)};
static constexpr std::string_view EKW_1748[] = {KW(5644),KW(5645),KW(3575),KW(2844),KW(2845),KW(3562),KW(5646),KW(5647)};
static constexpr std::string_view EKW_1749[] = {KW(5648),KW(5649),KW(3575),KW(2844),KW(2845),KW(3562),KW(5650),KW(5651)};
static constexpr std::string_view EKW_1750[] = {KW(5652),KW(5653),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5654),KW(5393)};
static constexpr std::string_view EKW_1751[] = {KW(5655),KW(3575),KW(2844),KW(2845),KW(3562),KW(5656),KW(5657)};
static constexpr std::string_view EKW_1752[] = {KW(5658),KW(5659),KW(3575),KW(2844),KW(2845),KW(3562),KW(2361),KW(5660)};
static constexpr std::string_view EKW_1753[] = {KW(5661),KW(5662),KW(3575),KW(2844),KW(2845),KW(3562),KW(5663),KW(5664)};
static constexpr std::string_view EKW_1754[] = {KW(5665),KW(5666),KW(1143),KW(3575),KW(2844),KW(2845),KW(3562),KW(5667),KW(5668)};
static constexpr std::string_view EKW_1755[] = {KW(5669),KW(5670),KW(3575),KW(2844),KW(2845),KW(3562),KW(2308),KW(1481)};
static constexpr std::string_view EKW_1756[] = {KW(5671),KW(1437),KW(1481),KW(1753),KW(5672),KW(3575),KW(2844),KW(2845),KW(3562),KW(5673),KW(5674),KW(5675),KW(5676),KW(5393)};
static constexpr std::string_view EKW_1757[] = {KW(5677),KW(5678),KW(3575),KW(2844),KW(2845),KW(3562),KW(5679),KW(5680)};
static constexpr std::string_view EKW_1758[] = {KW(5681),KW(5682),KW(5683),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(5684)};
static constexpr std::string_view EKW_1759[] = {KW(5685),KW(5686),KW(3575),KW(2844),KW(2845),KW(3562),KW(5687),KW(5688)};
static constexpr std::string_view EKW_1760[] = {KW(5689),KW(2547),KW(3575),KW(2844),KW(2845),KW(3562),KW(5429),KW(2638),KW(5024),KW(5430),KW(801)};
static constexpr std::string_view EKW_1761[] = {KW(5690),KW(5691),KW(3575),KW(2844),KW(2845),KW(3562),KW(5692)};
static constexpr std::string_view EKW_1762[] = {KW(5693),KW(5694),KW(3575),KW(2844),KW(2845),KW(3562),KW(5695),KW(5696)};
static constexpr std::string_view EKW_1763[] = {KW(5697),KW(5698),KW(3575),KW(2844),KW(2845),KW(3562),KW(5699),KW(5700)};
static constexpr std::string_view EKW_1764[] = {KW(5701),KW(1448),KW(2844),KW(3575),KW(2845),KW(3562),KW(2652),KW(5702),KW(5703),KW(5429),KW(5024),KW(5430),KW(801)};
static constexpr std::string_view EKW_1765[] = {KW(5704),KW(5705),KW(3575),KW(2844),KW(2845),KW(3562),KW(5706),KW(5707)};
static constexpr std::string_view EKW_1766[] = {KW(5708),KW(5709),KW(3575),KW(2844),KW(2845),KW(3562),KW(5710),KW(5711)};
static constexpr std::string_view EKW_1767[] = {KW(5712),KW(5713),KW(3575),KW(2844),KW(2845),KW(3562),KW(5714),KW(5715)};
static constexpr std::string_view EKW_1768[] = {KW(5716),KW(5717),KW(3575),KW(2844),KW(2845),KW(3562),KW(5718),KW(851)};
static constexpr std::string_view EKW_1769[] = {KW(5719),KW(5720),KW(3575),KW(2844),KW(2845),KW(3562),KW(5721),KW(5722)};
static constexpr std::string_view EKW_1770[] = {KW(5723),KW(1537),KW(5724),KW(5725),KW(3575),KW(2844),KW(2845),KW(3562),KW(5726),KW(5341)};
static constexpr std::string_view EKW_1771[] = {KW(5727),KW(4729),KW(5728),KW(2844),KW(3575),KW(2845),KW(3562),KW(5729),KW(5730)};
static constexpr std::string_view EKW_1772[] = {KW(5731),KW(4741),KW(5728),KW(2844),KW(3575),KW(2845),KW(3562),KW(5732),KW(5429),KW(5730),KW(5733),KW(5024),KW(5430),KW(801)};
static constexpr std::string_view EKW_1773[] = {KW(5734),KW(5735),KW(3575),KW(2844),KW(2845),KW(3562),KW(5736),KW(5737)};
static constexpr std::string_view EKW_1774[] = {KW(5738),KW(5739),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5740),KW(5741),KW(5393)};
static constexpr std::string_view EKW_1775[] = {KW(5742),KW(5743),KW(3575),KW(2844),KW(2845),KW(3562),KW(5744),KW(5745),KW(5746)};
static constexpr std::string_view EKW_1776[] = {KW(5747),KW(5748),KW(5396),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(5749),KW(5750)};
static constexpr std::string_view EKW_1777[] = {KW(5751),KW(5752),KW(3575),KW(2844),KW(2845),KW(3562),KW(5753),KW(5754)};
static constexpr std::string_view EKW_1778[] = {KW(5755),KW(1537),KW(5756),KW(3575),KW(2844),KW(2845),KW(3562),KW(5757),KW(5341)};
static constexpr std::string_view EKW_1779[] = {KW(5758),KW(5759),KW(3575),KW(2844),KW(2845),KW(3562),KW(5760),KW(5761)};
static constexpr std::string_view EKW_1780[] = {KW(5762),KW(5763),KW(5764),KW(3575),KW(2844),KW(2845),KW(3562),KW(5765),KW(5766)};
static constexpr std::string_view EKW_1781[] = {KW(5767),KW(5768),KW(3575),KW(2844),KW(2845),KW(3562),KW(5769),KW(5770)};
static constexpr std::string_view EKW_1782[] = {KW(5771),KW(5772),KW(3575),KW(2844),KW(2845),KW(3562),KW(5773),KW(5774)};
static constexpr std::string_view EKW_1783[] = {KW(5775),KW(5776),KW(3575),KW(2844),KW(2845),KW(3562),KW(5777),KW(5778)};
static constexpr std::string_view EKW_1784[] = {KW(5779),KW(5780),KW(3575),KW(2844),KW(2845),KW(3562),KW(5781),KW(5782)};
static constexpr std::string_view EKW_1785[] = {KW(5783),KW(5784),KW(3575),KW(2844),KW(2845),KW(3562),KW(5785),KW(5786)};
static constexpr std::string_view EKW_1786[] = {KW(5787),KW(5788),KW(3575),KW(2844),KW(2845),KW(3562),KW(5789),KW(5790)};
static constexpr std::string_view EKW_1787[] = {KW(5791),KW(921),KW(3575),KW(2844),KW(2845),KW(3562),KW(5792),KW(5793)};
static constexpr std::string_view EKW_1788[] = {KW(5794),KW(5795),KW(3575),KW(2844),KW(2845),KW(3562),KW(5796),KW(5797)};
static constexpr std::string_view EKW_1789[] = {KW(5798),KW(5799),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(5800)};
static constexpr std::string_view EKW_1790[] = {KW(5801),KW(5802),KW(3575),KW(2844),KW(2845),KW(3562),KW(5803),KW(5804)};
static constexpr std::string_view EKW_1791[] = {KW(5805),KW(5341)};
static constexpr std::string_view EKW_1792[] = {KW(5806),KW(5807),KW(3575),KW(2844),KW(2845),KW(3562),KW(5808),KW(5809)};
static constexpr std::string_view EKW_1793[] = {KW(5810),KW(5811),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5812),KW(5393),KW(5813)};
static constexpr std::string_view EKW_1794[] = {KW(5814),KW(5815),KW(3575),KW(2844),KW(2845),KW(3562),KW(5816),KW(5817)};
static constexpr std::string_view EKW_1795[] = {KW(5818),KW(5819),KW(3575),KW(2844),KW(2845),KW(3562),KW(5820),KW(5821)};
static constexpr std::string_view EKW_1796[] = {KW(5822),KW(5823),KW(3575),KW(2844),KW(2845),KW(3562),KW(5824),KW(5825),KW(5826),KW(4867),KW(5827)};
static constexpr std::string_view EKW_1797[] = {KW(5828),KW(5829),KW(3575),KW(2844),KW(2845),KW(3562),KW(5830),KW(5831)};
static constexpr std::string_view EKW_1798[] = {KW(5832),KW(5833),KW(3575),KW(2844),KW(2845),KW(3562),KW(5834),KW(5835),KW(5836)};
static constexpr std::string_view EKW_1799[] = {KW(5837),KW(5559),KW(5838),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(5839),KW(5393),KW(5544),KW(4729)};
static constexpr std::string_view EKW_1800[] = {KW(5840),KW(5841),KW(3575),KW(2844),KW(2845),KW(3562),KW(5842),KW(5843),KW(5844),KW(2185)};
static constexpr std::string_view EKW_1801[] = {KW(5845),KW(5846),KW(3575),KW(2844),KW(2845),KW(3562),KW(5847),KW(5848)};
static constexpr std::string_view EKW_1802[] = {KW(5849),KW(5850),KW(3575),KW(2844),KW(2845),KW(3562),KW(5851),KW(5852)};
static constexpr std::string_view EKW_1803[] = {KW(5853),KW(5854),KW(3575),KW(2844),KW(2845),KW(3562),KW(5855),KW(5856)};
static constexpr std::string_view EKW_1804[] = {KW(5857),KW(5858),KW(3575),KW(2844),KW(2845),KW(3562),KW(2147),KW(5859)};
static constexpr std::string_view EKW_1805[] = {KW(5860),KW(5861),KW(3575),KW(2844),KW(2845),KW(3562),KW(5862),KW(5863)};
static constexpr std::string_view EKW_1806[] = {KW(5864),KW(5865),KW(3575),KW(2844),KW(2845),KW(3562),KW(5866),KW(5867)};
static constexpr std::string_view EKW_1807[] = {KW(5868),KW(5869),KW(3575),KW(2844),KW(2845),KW(3562),KW(3756),KW(2562)};
static constexpr std::string_view EKW_1808[] = {KW(5870),KW(5871),KW(3575),KW(2844),KW(2845),KW(3562),KW(5872),KW(5873)};
static constexpr std::string_view EKW_1809[] = {KW(5874),KW(5875),KW(3575),KW(2844),KW(2845),KW(3562),KW(5876),KW(5877)};
static constexpr std::string_view EKW_1810[] = {KW(5878),KW(5879),KW(3575),KW(2844),KW(2845),KW(3562),KW(5880),KW(5881)};
static constexpr std::string_view EKW_1811[] = {KW(5882),KW(2968),KW(5883),KW(3575),KW(2844),KW(2845),KW(3562),KW(5884),KW(5885)};
static constexpr std::string_view EKW_1812[] = {KW(5886),KW(5887),KW(3575),KW(2844),KW(2845),KW(3562),KW(5888),KW(5889)};
static constexpr std::string_view EKW_1813[] = {KW(5890),KW(5891),KW(5393),KW(3575),KW(2844),KW(2845),KW(3562),KW(5892)};
static constexpr std::string_view EKW_1814[] = {KW(5893),KW(3575),KW(2844),KW(2845),KW(3562),KW(5894),KW(5895)};
static constexpr std::string_view EKW_1815[] = {KW(5896),KW(5897),KW(3575),KW(2844),KW(2845),KW(3562),KW(5898),KW(5899)};
static constexpr std::string_view EKW_1816[] = {KW(5900),KW(5901),KW(3575),KW(2844),KW(2845),KW(3562),KW(5902),KW(5903)};
static constexpr std::string_view EKW_1817[] = {KW(5904),KW(242),KW(3575),KW(2844),KW(2845),KW(3562),KW(5375),KW(5905),KW(5906),KW(5907),KW(5908),KW(5909)};
static constexpr std::string_view EKW_1818[] = {KW(5910),KW(5911),KW(3575),KW(2844),KW(2845),KW(3562),KW(5912),KW(5913)};
static constexpr std::string_view EKW_1819[] = {KW(5914),KW(5915),KW(3575),KW(2844),KW(2845),KW(3562),KW(5916),KW(5917)};
static constexpr std::string_view EKW_1820[] = {KW(5918),KW(5919),KW(3575),KW(2844),KW(2845),KW(3562),KW(5920),KW(5921)};
static constexpr std::string_view EKW_1821[] = {KW(5922),KW(2968),KW(5923),KW(3575),KW(2844),KW(2845),KW(3562),KW(5924),KW(5925)};
static constexpr std::string_view EKW_1822[] = {KW(5926),KW(5927),KW(3575),KW(2844),KW(2845),KW(3562),KW(5928),KW(5929)};
static constexpr std::string_view EKW_1823[] = {KW(5930),KW(3989),KW(3575),KW(2844),KW(2845),KW(3562),KW(5931),KW(5932)};
static constexpr std::string_view EKW_1824[] = {KW(5933),KW(5934),KW(3575),KW(2844),KW(2845),KW(3562),KW(5935),KW(5936)};
static constexpr std::string_view EKW_1825[] = {KW(5937),KW(2486),KW(5938),KW(3575),KW(2844),KW(2845),KW(3562),KW(5939),KW(5940)};
static constexpr std::string_view EKW_1826[] = {KW(5941),KW(5942),KW(2968),KW(5600),KW(3575),KW(2844),KW(2845),KW(3562),KW(5943),KW(5578),KW(5944)};
static constexpr std::string_view EKW_1827[] = {KW(5945),KW(5946),KW(3575),KW(2844),KW(2845),KW(3562),KW(5947)};
static constexpr std::string_view EKW_1828[] = {KW(5948),KW(5949),KW(3575),KW(2844),KW(2845),KW(3562),KW(5950),KW(5951)};
static constexpr std::string_view EKW_1829[] = {KW(5952),KW(5953),KW(3575),KW(2844),KW(2845),KW(3562),KW(5954),KW(5955)};
static constexpr std::string_view EKW_1830[] = {KW(5956),KW(1537),KW(5957),KW(5958),KW(3575),KW(2844),KW(2845),KW(3562),KW(5959),KW(5341)};
static constexpr std::string_view EKW_1831[] = {KW(5960),KW(5961),KW(3575),KW(2844),KW(2845),KW(3562),KW(5962),KW(5393)};
static constexpr std::string_view EKW_1832[] = {KW(5963),KW(5964),KW(5965),KW(3575),KW(2844),KW(2845),KW(3562),KW(5966),KW(5442)};
static constexpr std::string_view EKW_1833[] = {KW(5967),KW(5968),KW(5969),KW(5970),KW(3575),KW(2844),KW(2845),KW(3562),KW(5971)};
static constexpr std::string_view EKW_1834[] = {KW(5972),KW(5973),KW(3575),KW(2844),KW(2845),KW(3562),KW(5974),KW(5975)};
static constexpr std::string_view EKW_1835[] = {KW(5976),KW(5977),KW(3575),KW(2844),KW(2845),KW(3562),KW(5978),KW(5979)};
static constexpr std::string_view EKW_1836[] = {KW(5980),KW(5981),KW(3575),KW(2844),KW(2845),KW(3562),KW(5982),KW(5983)};
static constexpr std::string_view EKW_1837[] = {KW(5984),KW(5985),KW(3575),KW(2844),KW(2845),KW(3562),KW(5986),KW(5987)};
static constexpr std::string_view EKW_1838[] = {KW(5988),KW(5989),KW(3575),KW(2844),KW(2845),KW(3562),KW(5990),KW(5991)};
static constexpr std::string_view EKW_1839[] = {KW(5992),KW(5993),KW(3575),KW(2844),KW(2845),KW(3562),KW(5994),KW(5995)};
static constexpr std::string_view EKW_1840[] = {KW(5996),KW(5997),KW(3575),KW(2844),KW(2845),KW(3562),KW(5998),KW(5999)};
static constexpr std::string_view EKW_1841[] = {KW(6000),KW(3762),KW(6001),KW(3575),KW(2844),KW(2845),KW(3562),KW(3761),KW(5429),KW(5024),KW(5430),KW(6002),KW(801)};
static constexpr std::string_view EKW_1842[] = {KW(6003),KW(6004),KW(3575),KW(2844),KW(2845),KW(3562),KW(6005),KW(6006)};
static constexpr std::string_view EKW_1843[] = {KW(6007),KW(3575),KW(2844),KW(2845),KW(3562),KW(6008),KW(6009)};
static constexpr std::string_view EKW_1844[] = {KW(6010),KW(6011),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(6012),KW(5393),KW(6013)};
static constexpr std::string_view EKW_1845[] = {KW(6014),KW(6015),KW(3575),KW(2844),KW(2845),KW(3562),KW(6016),KW(6017)};
static constexpr std::string_view EKW_1846[] = {KW(6018),KW(6019),KW(3575),KW(2844),KW(2845),KW(3562),KW(6020),KW(6021)};
static constexpr std::string_view EKW_1847[] = {KW(6022),KW(6023),KW(3575),KW(2844),KW(2845),KW(3562),KW(1991),KW(6024)};
static constexpr std::string_view EKW_1848[] = {KW(6025),KW(6026),KW(3575),KW(2844),KW(2845),KW(3562),KW(6027),KW(6028)};
static constexpr std::string_view EKW_1849[] = {KW(6029),KW(1537),KW(6030),KW(6031),KW(6032),KW(6033),KW(3575),KW(2844),KW(2845),KW(3562),KW(6034),KW(5341)};
static constexpr std::string_view EKW_1850[] = {KW(6035),KW(6036),KW(3575),KW(2844),KW(2845),KW(3562),KW(6037),KW(6038)};
static constexpr std::string_view EKW_1851[] = {KW(6039)};
static constexpr std::string_view EKW_1852[] = {KW(6040),KW(6041),KW(3575),KW(2844),KW(2845),KW(3562),KW(6042),KW(6043),KW(6044)};
static constexpr std::string_view EKW_1853[] = {KW(6045),KW(6046),KW(6047),KW(3575),KW(2844),KW(2845),KW(3562),KW(6048),KW(6049)};
static constexpr std::string_view EKW_1854[] = {KW(6050),KW(2860),KW(6051),KW(3575),KW(2844),KW(2845),KW(3562),KW(6052),KW(6053)};
static constexpr std::string_view EKW_1855[] = {KW(6054),KW(6055),KW(3575),KW(2844),KW(2845),KW(3562),KW(6056),KW(6057)};
static constexpr std::string_view EKW_1856[] = {KW(6058),KW(6059),KW(3575),KW(2844),KW(2845),KW(3562),KW(6060),KW(6061)};
static constexpr std::string_view EKW_1857[] = {KW(6062),KW(6063),KW(3575),KW(2844),KW(2845),KW(3562),KW(6064),KW(6065)};
static constexpr std::string_view EKW_1858[] = {KW(6066),KW(4741),KW(6019),KW(3575),KW(2844),KW(2845),KW(3562),KW(6067),KW(6068)};
static constexpr std::string_view EKW_1859[] = {KW(6069),KW(6070),KW(6071),KW(6072),KW(3575),KW(2844),KW(2845),KW(3562),KW(6073),KW(6074),KW(6075),KW(6076)};
static constexpr std::string_view EKW_1860[] = {KW(6077),KW(6078),KW(6079),KW(3575),KW(2844),KW(2845),KW(3562),KW(6080),KW(6081)};
static constexpr std::string_view EKW_1861[] = {KW(6082),KW(5360),KW(6083),KW(5903),KW(3575),KW(2844),KW(2845),KW(3562),KW(6084)};
static constexpr std::string_view EKW_1862[] = {KW(6085),KW(6086),KW(1477),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(6087)};
static constexpr std::string_view EKW_1863[] = {KW(6088),KW(6089),KW(3575),KW(2844),KW(2845),KW(3562),KW(6090),KW(6091)};
static constexpr std::string_view EKW_1864[] = {KW(6092)};
static constexpr std::string_view EKW_1865[] = {KW(6093),KW(6094),KW(6095),KW(5293),KW(3575),KW(2844),KW(2845),KW(3562),KW(6096),KW(5393)};
static constexpr std::string_view EKW_1866[] = {KW(6097),KW(6098),KW(3575),KW(2844),KW(2845),KW(3562),KW(6099),KW(6100)};
static constexpr std::string_view EKW_1867[] = {KW(6101),KW(2486),KW(6102),KW(5970),KW(3575),KW(2844),KW(2845),KW(3562),KW(6103),KW(5269),KW(6104)};
static constexpr std::string_view EKW_1868[] = {KW(6105),KW(6106),KW(3575),KW(2844),KW(2845),KW(3562),KW(6107),KW(6108)};
static constexpr std::string_view EKW_1869[] = {KW(6109),KW(2061),KW(3575),KW(2844),KW(2845),KW(3562),KW(6110),KW(6111)};
static constexpr std::string_view EKW_1870[] = {KW(6112),KW(6113),KW(3575),KW(2844),KW(2845),KW(3562),KW(6114),KW(6115)};
static constexpr std::string_view EKW_1871[] = {KW(6116),KW(6117),KW(3575),KW(2844),KW(2845),KW(3562),KW(6118),KW(6119)};
static constexpr std::string_view EKW_1872[] = {KW(6120),KW(6121),KW(6122),KW(3575),KW(2844),KW(2845),KW(3562),KW(6123),KW(2830),KW(6124),KW(6125)};
static constexpr std::string_view EKW_1873[] = {KW(6126),KW(3575),KW(2844),KW(2845),KW(3562),KW(6127),KW(6128)};
static constexpr std::string_view EKW_1874[] = {KW(6129),KW(6130),KW(3575),KW(2844),KW(2845),KW(3562),KW(6131),KW(6132)};
static constexpr std::string_view EKW_1875[] = {KW(6133),KW(6134),KW(3575),KW(2844),KW(2845),KW(3562),KW(6135),KW(6136)};
static constexpr std::string_view EKW_1876[] = {KW(6137),KW(2118),KW(3575),KW(2844),KW(2845),KW(3562),KW(6138),KW(6139),KW(6140)};
static constexpr std::string_view EKW_1877[] = {KW(6141),KW(6142),KW(6143),KW(3575),KW(2844),KW(2845),KW(3562),KW(6144)};
static constexpr std::string_view EKW_1878[] = {KW(6145),KW(3575),KW(2844),KW(2845),KW(3562),KW(6146),KW(6147)};
static constexpr std::string_view EKW_1879[] = {KW(6148),KW(6149),KW(3575),KW(2844),KW(2845),KW(3562),KW(2779),KW(5426),KW(6150)};
static constexpr std::string_view EKW_1880[] = {KW(6151),KW(6152),KW(5238),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(6153)};
static constexpr std::string_view EKW_1881[] = {KW(6154),KW(6155),KW(3575),KW(2844),KW(2845),KW(3562),KW(6156),KW(6157)};
static constexpr std::string_view EKW_1882[] = {KW(6158),KW(6159),KW(3575),KW(2844),KW(2845),KW(3562),KW(6160),KW(6161)};
static constexpr std::string_view EKW_1883[] = {KW(6162),KW(6163),KW(6164)};
static constexpr std::string_view EKW_1884[] = {KW(6165),KW(6166),KW(3575),KW(3562)};
static constexpr std::string_view EKW_1885[] = {KW(6167),KW(5238),KW(5543),KW(2552),KW(3575),KW(2844),KW(2845),KW(3562),KW(6168),KW(2966),KW(5429),KW(5293),KW(5024),KW(6169),KW(5430),KW(801),KW(6164),KW(6170)};
static constexpr std::string_view EKW_1886[] = {KW(6171),KW(6172),KW(3575),KW(2844),KW(2845),KW(3562),KW(6173),KW(6174)};
static constexpr std::string_view EKW_1887[] = {KW(6175),KW(6176),KW(3575),KW(2844),KW(2845),KW(3562),KW(6177),KW(6178),KW(6179)};
static constexpr std::string_view EKW_1888[] = {KW(6180),KW(6181),KW(2913),KW(3575),KW(2844),KW(2845),KW(3562),KW(6182),KW(6183)};
static constexpr std::string_view EKW_1889[] = {KW(6184),KW(1537),KW(6185),KW(6186),KW(3575),KW(2844),KW(2845),KW(3562),KW(6187),KW(5341)};
static constexpr std::string_view EKW_1890[] = {KW(6188),KW(6189),KW(6190),KW(5397),KW(3575),KW(2844),KW(2845),KW(3562),KW(6191),KW(6192)};
static constexpr std::string_view EKW_1891[] = {KW(6193),KW(1437),KW(1150),KW(5293),KW(6194),KW(3575),KW(2844),KW(2845),KW(3562),KW(6195),KW(5393),KW(5417)};
static constexpr std::string_view EKW_1892[] = {KW(6196),KW(1150),KW(5293),KW(6164),KW(3575),KW(2844),KW(2845),KW(3562),KW(6197),KW(2552),KW(5393),KW(5417),KW(5543),KW(6163),KW(5238),KW(6170)};
static constexpr std::string_view EKW_1893[] = {KW(6198),KW(6199),KW(6200),KW(3575),KW(2844),KW(2845),KW(3562),KW(2310),KW(6201)};
static constexpr std::string_view EKW_1894[] = {KW(6202),KW(6203),KW(3575),KW(2844),KW(2845),KW(3562),KW(6204),KW(5897),KW(6205)};
static constexpr std::string_view EKW_1895[] = {KW(6206),KW(6207),KW(6208),KW(3575),KW(2844),KW(2845),KW(3562),KW(6209)};
static constexpr std::string_view EKW_1896[] = {KW(6210),KW(5275),KW(3575),KW(2844),KW(2845),KW(3562),KW(6211),KW(6212)};
static constexpr std::string_view EKW_1897[] = {KW(6213),KW(6214),KW(3575),KW(2844),KW(2845),KW(3562),KW(6215),KW(6216)};
static constexpr std::string_view EKW_1898[] = {KW(6217),KW(6218),KW(3575),KW(2844),KW(2845),KW(3562),KW(6219),KW(6220)};
static constexpr std::string_view EKW_1899[] = {KW(6221),KW(6222),KW(3575),KW(2844),KW(2845),KW(3562),KW(6223)};
static constexpr std::string_view EKW_1900[] = {KW(6224),KW(4741),KW(6225),KW(3575),KW(2844),KW(2845),KW(3562),KW(6226),KW(6227)};
static constexpr std::string_view EKW_1901[] = {KW(6228),KW(6229),KW(3575),KW(2844),KW(2845),KW(3562),KW(6230),KW(6231)};
static constexpr std::string_view EKW_1902[] = {KW(6232),KW(6233),KW(3575),KW(2844),KW(2845),KW(3562),KW(6234),KW(6235),KW(6236)};
static constexpr std::string_view EKW_1903[] = {KW(6237),KW(3575),KW(5561),KW(403),KW(6238),KW(6239)};
static constexpr std::string_view EKW_1904[] = {KW(6240),KW(3575),KW(6241),KW(6242),KW(403),KW(6243),KW(6239)};
static constexpr std::string_view EKW_1905[] = {KW(6244),KW(3575),KW(6245),KW(6246),KW(6247),KW(6248),KW(2193),KW(6249),KW(381),KW(6250)};

static const std::array<EmojiData, 1906> EMOJI_LIST = {
	EmojiData{ .emoji = "😀", .name = "grinning face", .group = GRP(0), .keywords = EKW_0, .skinToneSupport = false},
	EmojiData{ .emoji = "😃", .name = "grinning face with big eyes", .group = GRP(0), .keywords = EKW_1, .skinToneSupport = false},
	EmojiData{ .emoji = "😄", .name = "grinning face with smiling eyes", .group = GRP(0), .keywords = EKW_2, .skinToneSupport = false},
	EmojiData{ .emoji = "😁", .name = "beaming face with smiling eyes", .group = GRP(0), .keywords = EKW_3, .skinToneSupport = false},
	EmojiData{ .emoji = "😆", .name = "grinning squinting face", .group = GRP(0), .keywords = EKW_4, .skinToneSupport = false},
	EmojiData{ .emoji = "😅", .name = "grinning face with sweat", .group = GRP(0), .keywords = EKW_5, .skinToneSupport = false},
	EmojiData{ .emoji = "🤣", .name = "rolling on the floor laughing", .group = GRP(0), .keywords = EKW_6, .skinToneSupport = false},
	EmojiData{ .emoji = "😂", .name = "face with tears of joy", .group = GRP(0), .keywords = EKW_7, .skinToneSupport = false},
	EmojiData{ .emoji = "🙂", .name = "slightly smiling face", .group = GRP(0), .keywords = EKW_8, .skinToneSupport = false},
	EmojiData{ .emoji = "🙃", .name = "upside-down face", .group = GRP(0), .keywords = EKW_9, .skinToneSupport = false},
	EmojiData{ .emoji = "🫠", .name = "melting face", .group = GRP(0), .keywords = EKW_10, .skinToneSupport = false},
	EmojiData{ .emoji = "😉", .name = "winking face", .group = GRP(0), .keywords = EKW_11, .skinToneSupport = false},
	EmojiData{ .emoji = "😊", .name = "smiling face with smiling eyes", .group = GRP(0), .keywords = EKW_12, .skinToneSupport = false},
	EmojiData{ .emoji = "😇", .name = "smiling face with halo", .group = GRP(0), .keywords = EKW_13, .skinToneSupport = false},
	EmojiData{ .emoji = "🥰", .name = "smiling face with hearts", .group = GRP(0), .keywords = EKW_14, .skinToneSupport = false},
	EmojiData{ .emoji = "😍", .name = "smiling face with heart-eyes", .group = GRP(0), .keywords = EKW_15, .skinToneSupport = false},
	EmojiData{ .emoji = "🤩", .name = "star-struck", .group = GRP(0), .keywords = EKW_16, .skinToneSupport = false},
	EmojiData{ .emoji = "😘", .name = "face blowing a kiss", .group = GRP(0), .keywords = EKW_17, .skinToneSupport = false},
	EmojiData{ .emoji = "😗", .name = "kissing face", .group = GRP(0), .keywords = EKW_18, .skinToneSupport = false},
	EmojiData{ .emoji = "☺️", .name = "smiling face", .group = GRP(0), .keywords = EKW_19, .skinToneSupport = false},
	EmojiData{ .emoji = "😚", .name = "kissing face with closed eyes", .group = GRP(0), .keywords = EKW_20, .skinToneSupport = false},
	EmojiData{ .emoji = "😙", .name = "kissing face with smiling eyes", .group = GRP(0), .keywords = EKW_21, .skinToneSupport = false},
	EmojiData{ .emoji = "🥲", .name = "smiling face with tear", .group = GRP(0), .keywords = EKW_22, .skinToneSupport = false},
	EmojiData{ .emoji = "😋", .name = "face savoring food", .group = GRP(0), .keywords = EKW_23, .skinToneSupport = false},
	EmojiData{ .emoji = "😛", .name = "face with tongue", .group = GRP(0), .keywords = EKW_24, .skinToneSupport = false},
	EmojiData{ .emoji = "😜", .name = "winking face with tongue", .group = GRP(0), .keywords = EKW_25, .skinToneSupport = false},
	EmojiData{ .emoji = "🤪", .name = "zany face", .group = GRP(0), .keywords = EKW_26, .skinToneSupport = false},
	EmojiData{ .emoji = "😝", .name = "squinting face with tongue", .group = GRP(0), .keywords = EKW_27, .skinToneSupport = false},
	EmojiData{ .emoji = "🤑", .name = "money-mouth face", .group = GRP(0), .keywords = EKW_28, .skinToneSupport = false},
	EmojiData{ .emoji = "🤗", .name = "smiling face with open hands", .group = GRP(0), .keywords = EKW_29, .skinToneSupport = false},
	EmojiData{ .emoji = "🤭", .name = "face with hand over mouth", .group = GRP(0), .keywords = EKW_30, .skinToneSupport = false},
	EmojiData{ .emoji = "🫢", .name = "face with open eyes and hand over mouth", .group = GRP(0), .keywords = EKW_31, .skinToneSupport = false},
	EmojiData{ .emoji = "🫣", .name = "face with peeking eye", .group = GRP(0), .keywords = EKW_32, .skinToneSupport = false},
	EmojiData{ .emoji = "🤫", .name = "shushing face", .group = GRP(0), .keywords = EKW_33, .skinToneSupport = false},
	EmojiData{ .emoji = "🤔", .name = "thinking face", .group = GRP(0), .keywords = EKW_34, .skinToneSupport = false},
	EmojiData{ .emoji = "🫡", .name = "saluting face", .group = GRP(0), .keywords = EKW_35, .skinToneSupport = false},
	EmojiData{ .emoji = "🤐", .name = "zipper-mouth face", .group = GRP(0), .keywords = EKW_36, .skinToneSupport = false},
	EmojiData{ .emoji = "🤨", .name = "face with raised eyebrow", .group = GRP(0), .keywords = EKW_37, .skinToneSupport = false},
	EmojiData{ .emoji = "😐", .name = "neutral face", .group = GRP(0), .keywords = EKW_38, .skinToneSupport = false},
	EmojiData{ .emoji = "😑", .name = "expressionless face", .group = GRP(0), .keywords = EKW_39, .skinToneSupport = false},
	EmojiData{ .emoji = "😶", .name = "face without mouth", .group = GRP(0), .keywords = EKW_40, .skinToneSupport = false},
	EmojiData{ .emoji = "🫥", .name = "dotted line face", .group = GRP(0), .keywords = EKW_41, .skinToneSupport = false},
	EmojiData{ .emoji = "😶‍🌫️", .name = "face in clouds", .group = GRP(0), .keywords = EKW_42, .skinToneSupport = false},
	EmojiData{ .emoji = "😏", .name = "smirking face", .group = GRP(0), .keywords = EKW_43, .skinToneSupport = false},
	EmojiData{ .emoji = "😒", .name = "unamused face", .group = GRP(0), .keywords = EKW_44, .skinToneSupport = false},
	EmojiData{ .emoji = "🙄", .name = "face with rolling eyes", .group = GRP(0), .keywords = EKW_45, .skinToneSupport = false},
	EmojiData{ .emoji = "😬", .name = "grimacing face", .group = GRP(0), .keywords = EKW_46, .skinToneSupport = false},
	EmojiData{ .emoji = "😮‍💨", .name = "face exhaling", .group = GRP(0), .keywords = EKW_47, .skinToneSupport = false},
	EmojiData{ .emoji = "🤥", .name = "lying face", .group = GRP(0), .keywords = EKW_48, .skinToneSupport = false},
	EmojiData{ .emoji = "🫨", .name = "shaking face", .group = GRP(0), .keywords = EKW_49, .skinToneSupport = false},
	EmojiData{ .emoji = "🙂‍↔️", .name = "head shaking horizontally", .group = GRP(0), .keywords = EKW_50, .skinToneSupport = false},
	EmojiData{ .emoji = "🙂‍↕️", .name = "head shaking vertically", .group = GRP(0), .keywords = EKW_51, .skinToneSupport = false},
	EmojiData{ .emoji = "😌", .name = "relieved face", .group = GRP(0), .keywords = EKW_52, .skinToneSupport = false},
	EmojiData{ .emoji = "😔", .name = "pensive face", .group = GRP(0), .keywords = EKW_53, .skinToneSupport = false},
	EmojiData{ .emoji = "😪", .name = "sleepy face", .group = GRP(0), .keywords = EKW_54, .skinToneSupport = false},
	EmojiData{ .emoji = "🤤", .name = "drooling face", .group = GRP(0), .keywords = EKW_55, .skinToneSupport = false},
	EmojiData{ .emoji = "😴", .name = "sleeping face", .group = GRP(0), .keywords = EKW_56, .skinToneSupport = false},
	EmojiData{ .emoji = "🫩", .name = "face with bags under eyes", .group = GRP(0), .keywords = EKW_57, .skinToneSupport = false},
	EmojiData{ .emoji = "😷", .name = "face with medical mask", .group = GRP(0), .keywords = EKW_58, .skinToneSupport = false},
	EmojiData{ .emoji = "🤒", .name = "face with thermometer", .group = GRP(0), .keywords = EKW_59, .skinToneSupport = false},
	EmojiData{ .emoji = "🤕", .name = "face with head-bandage", .group = GRP(0), .keywords = EKW_60, .skinToneSupport = false},
	EmojiData{ .emoji = "🤢", .name = "nauseated face", .group = GRP(0), .keywords = EKW_61, .skinToneSupport = false},
	EmojiData{ .emoji = "🤮", .name = "face vomiting", .group = GRP(0), .keywords = EKW_62, .skinToneSupport = false},
	EmojiData{ .emoji = "🤧", .name = "sneezing face", .group = GRP(0), .keywords = EKW_63, .skinToneSupport = false},
	EmojiData{ .emoji = "🥵", .name = "hot face", .group = GRP(0), .keywords = EKW_64, .skinToneSupport = false},
	EmojiData{ .emoji = "🥶", .name = "cold face", .group = GRP(0), .keywords = EKW_65, .skinToneSupport = false},
	EmojiData{ .emoji = "🥴", .name = "woozy face", .group = GRP(0), .keywords = EKW_66, .skinToneSupport = false},
	EmojiData{ .emoji = "😵", .name = "face with crossed-out eyes", .group = GRP(0), .keywords = EKW_67, .skinToneSupport = false},
	EmojiData{ .emoji = "😵‍💫", .name = "face with spiral eyes", .group = GRP(0), .keywords = EKW_68, .skinToneSupport = false},
	EmojiData{ .emoji = "🤯", .name = "exploding head", .group = GRP(0), .keywords = EKW_69, .skinToneSupport = false},
	EmojiData{ .emoji = "🤠", .name = "cowboy hat face", .group = GRP(0), .keywords = EKW_70, .skinToneSupport = false},
	EmojiData{ .emoji = "🥳", .name = "partying face", .group = GRP(0), .keywords = EKW_71, .skinToneSupport = false},
	EmojiData{ .emoji = "🥸", .name = "disguised face", .group = GRP(0), .keywords = EKW_72, .skinToneSupport = false},
	EmojiData{ .emoji = "😎", .name = "smiling face with sunglasses", .group = GRP(0), .keywords = EKW_73, .skinToneSupport = false},
	EmojiData{ .emoji = "🤓", .name = "nerd face", .group = GRP(0), .keywords = EKW_74, .skinToneSupport = false},
	EmojiData{ .emoji = "🧐", .name = "face with monocle", .group = GRP(0), .keywords = EKW_75, .skinToneSupport = false},
	EmojiData{ .emoji = "😕", .name = "confused face", .group = GRP(0), .keywords = EKW_76, .skinToneSupport = false},
	EmojiData{ .emoji = "🫤", .name = "face with diagonal mouth", .group = GRP(0), .keywords = EKW_77, .skinToneSupport = false},
	EmojiData{ .emoji = "😟", .name = "worried face", .group = GRP(0), .keywords = EKW_78, .skinToneSupport = false},
	EmojiData{ .emoji = "🙁", .name = "slightly frowning face", .group = GRP(0), .keywords = EKW_79, .skinToneSupport = false},
	EmojiData{ .emoji = "☹️", .name = "frowning face", .group = GRP(0), .keywords = EKW_80, .skinToneSupport = false},
	EmojiData{ .emoji = "😮", .name = "face with open mouth", .group = GRP(0), .keywords = EKW_81, .skinToneSupport = false},
	EmojiData{ .emoji = "😯", .name = "hushed face", .group = GRP(0), .keywords = EKW_82, .skinToneSupport = false},
	EmojiData{ .emoji = "😲", .name = "astonished face", .group = GRP(0), .keywords = EKW_83, .skinToneSupport = false},
	EmojiData{ .emoji = "😳", .name = "flushed face", .group = GRP(0), .keywords = EKW_84, .skinToneSupport = false},
	EmojiData{ .emoji = "🥺", .name = "pleading face", .group = GRP(0), .keywords = EKW_85, .skinToneSupport = false},
	EmojiData{ .emoji = "🥹", .name = "face holding back tears", .group = GRP(0), .keywords = EKW_86, .skinToneSupport = false},
	EmojiData{ .emoji = "😦", .name = "frowning face with open mouth", .group = GRP(0), .keywords = EKW_87, .skinToneSupport = false},
	EmojiData{ .emoji = "😧", .name = "anguished face", .group = GRP(0), .keywords = EKW_88, .skinToneSupport = false},
	EmojiData{ .emoji = "😨", .name = "fearful face", .group = GRP(0), .keywords = EKW_89, .skinToneSupport = false},
	EmojiData{ .emoji = "😰", .name = "anxious face with sweat", .group = GRP(0), .keywords = EKW_90, .skinToneSupport = false},
	EmojiData{ .emoji = "😥", .name = "sad but relieved face", .group = GRP(0), .keywords = EKW_91, .skinToneSupport = false},
	EmojiData{ .emoji = "😢", .name = "crying face", .group = GRP(0), .keywords = EKW_92, .skinToneSupport = false},
	EmojiData{ .emoji = "😭", .name = "loudly crying face", .group = GRP(0), .keywords = EKW_93, .skinToneSupport = false},
	EmojiData{ .emoji = "😱", .name = "face screaming in fear", .group = GRP(0), .keywords = EKW_94, .skinToneSupport = false},
	EmojiData{ .emoji = "😖", .name = "confounded face", .group = GRP(0), .keywords = EKW_95, .skinToneSupport = false},
	EmojiData{ .emoji = "😣", .name = "persevering face", .group = GRP(0), .keywords = EKW_96, .skinToneSupport = false},
	EmojiData{ .emoji = "😞", .name = "disappointed face", .group = GRP(0), .keywords = EKW_97, .skinToneSupport = false},
	EmojiData{ .emoji = "😓", .name = "downcast face with sweat", .group = GRP(0), .keywords = EKW_98, .skinToneSupport = false},
	EmojiData{ .emoji = "😩", .name = "weary face", .group = GRP(0), .keywords = EKW_99, .skinToneSupport = false},
	EmojiData{ .emoji = "😫", .name = "tired face", .group = GRP(0), .keywords = EKW_100, .skinToneSupport = false},
	EmojiData{ .emoji = "🥱", .name = "yawning face", .group = GRP(0), .keywords = EKW_101, .skinToneSupport = false},
	EmojiData{ .emoji = "😤", .name = "face with steam from nose", .group = GRP(0), .keywords = EKW_102, .skinToneSupport = false},
	EmojiData{ .emoji = "😡", .name = "enraged face", .group = GRP(0), .keywords = EKW_103, .skinToneSupport = false},
	EmojiData{ .emoji = "😠", .name = "angry face", .group = GRP(0), .keywords = EKW_104, .skinToneSupport = false},
	EmojiData{ .emoji = "🤬", .name = "face with symbols on mouth", .group = GRP(0), .keywords = EKW_105, .skinToneSupport = false},
	EmojiData{ .emoji = "😈", .name = "smiling face with horns", .group = GRP(0), .keywords = EKW_106, .skinToneSupport = false},
	EmojiData{ .emoji = "👿", .name = "angry face with horns", .group = GRP(0), .keywords = EKW_107, .skinToneSupport = false},
	EmojiData{ .emoji = "💀", .name = "skull", .group = GRP(0), .keywords = EKW_108, .skinToneSupport = false},
	EmojiData{ .emoji = "☠️", .name = "skull and crossbones", .group = GRP(0), .keywords = EKW_109, .skinToneSupport = false},
	EmojiData{ .emoji = "💩", .name = "pile of poo", .group = GRP(0), .keywords = EKW_110, .skinToneSupport = false},
	EmojiData{ .emoji = "🤡", .name = "clown face", .group = GRP(0), .keywords = EKW_111, .skinToneSupport = false},
	EmojiData{ .emoji = "👹", .name = "ogre", .group = GRP(0), .keywords = EKW_112, .skinToneSupport = false},
	EmojiData{ .emoji = "👺", .name = "goblin", .group = GRP(0), .keywords = EKW_113, .skinToneSupport = false},
	EmojiData{ .emoji = "👻", .name = "ghost", .group = GRP(0), .keywords = EKW_114, .skinToneSupport = false},
	EmojiData{ .emoji = "👽", .name = "alien", .group = GRP(0), .keywords = EKW_115, .skinToneSupport = false},
	EmojiData{ .emoji = "👾", .name = "alien monster", .group = GRP(0), .keywords = EKW_116, .skinToneSupport = false},
	EmojiData{ .emoji = "🤖", .name = "robot", .group = GRP(0), .keywords = EKW_117, .skinToneSupport = false},
	EmojiData{ .emoji = "😺", .name = "grinning cat", .group = GRP(0), .keywords = EKW_118, .skinToneSupport = false},
	EmojiData{ .emoji = "😸", .name = "grinning cat with smiling eyes", .group = GRP(0), .keywords = EKW_119, .skinToneSupport = false},
	EmojiData{ .emoji = "😹", .name = "cat with tears of joy", .group = GRP(0), .keywords = EKW_120, .skinToneSupport = false},
	EmojiData{ .emoji = "😻", .name = "smiling cat with heart-eyes", .group = GRP(0), .keywords = EKW_121, .skinToneSupport = false},
	EmojiData{ .emoji = "😼", .name = "cat with wry smile", .group = GRP(0), .keywords = EKW_122, .skinToneSupport = false},
	EmojiData{ .emoji = "😽", .name = "kissing cat", .group = GRP(0), .keywords = EKW_123, .skinToneSupport = false},
	EmojiData{ .emoji = "🙀", .name = "weary cat", .group = GRP(0), .keywords = EKW_124, .skinToneSupport = false},
	EmojiData{ .emoji = "😿", .name = "crying cat", .group = GRP(0), .keywords = EKW_125, .skinToneSupport = false},
	EmojiData{ .emoji = "😾", .name = "pouting cat", .group = GRP(0), .keywords = EKW_126, .skinToneSupport = false},
	EmojiData{ .emoji = "🙈", .name = "see-no-evil monkey", .group = GRP(0), .keywords = EKW_127, .skinToneSupport = false},
	EmojiData{ .emoji = "🙉", .name = "hear-no-evil monkey", .group = GRP(0), .keywords = EKW_128, .skinToneSupport = false},
	EmojiData{ .emoji = "🙊", .name = "speak-no-evil monkey", .group = GRP(0), .keywords = EKW_129, .skinToneSupport = false},
	EmojiData{ .emoji = "💌", .name = "love letter", .group = GRP(0), .keywords = EKW_130, .skinToneSupport = false},
	EmojiData{ .emoji = "💘", .name = "heart with arrow", .group = GRP(0), .keywords = EKW_131, .skinToneSupport = false},
	EmojiData{ .emoji = "💝", .name = "heart with ribbon", .group = GRP(0), .keywords = EKW_132, .skinToneSupport = false},
	EmojiData{ .emoji = "💖", .name = "sparkling heart", .group = GRP(0), .keywords = EKW_133, .skinToneSupport = false},
	EmojiData{ .emoji = "💗", .name = "growing heart", .group = GRP(0), .keywords = EKW_134, .skinToneSupport = false},
	EmojiData{ .emoji = "💓", .name = "beating heart", .group = GRP(0), .keywords = EKW_135, .skinToneSupport = false},
	EmojiData{ .emoji = "💞", .name = "revolving hearts", .group = GRP(0), .keywords = EKW_136, .skinToneSupport = false},
	EmojiData{ .emoji = "💕", .name = "two hearts", .group = GRP(0), .keywords = EKW_137, .skinToneSupport = false},
	EmojiData{ .emoji = "💟", .name = "heart decoration", .group = GRP(0), .keywords = EKW_138, .skinToneSupport = false},
	EmojiData{ .emoji = "❣️", .name = "heart exclamation", .group = GRP(0), .keywords = EKW_139, .skinToneSupport = false},
	EmojiData{ .emoji = "💔", .name = "broken heart", .group = GRP(0), .keywords = EKW_140, .skinToneSupport = false},
	EmojiData{ .emoji = "❤️‍🔥", .name = "heart on fire", .group = GRP(0), .keywords = EKW_141, .skinToneSupport = false},
	EmojiData{ .emoji = "❤️‍🩹", .name = "mending heart", .group = GRP(0), .keywords = EKW_142, .skinToneSupport = false},
	EmojiData{ .emoji = "❤️", .name = "red heart", .group = GRP(0), .keywords = EKW_143, .skinToneSupport = false},
	EmojiData{ .emoji = "🩷", .name = "pink heart", .group = GRP(0), .keywords = EKW_144, .skinToneSupport = false},
	EmojiData{ .emoji = "🧡", .name = "orange heart", .group = GRP(0), .keywords = EKW_145, .skinToneSupport = false},
	EmojiData{ .emoji = "💛", .name = "yellow heart", .group = GRP(0), .keywords = EKW_146, .skinToneSupport = false},
	EmojiData{ .emoji = "💚", .name = "green heart", .group = GRP(0), .keywords = EKW_147, .skinToneSupport = false},
	EmojiData{ .emoji = "💙", .name = "blue heart", .group = GRP(0), .keywords = EKW_148, .skinToneSupport = false},
	EmojiData{ .emoji = "🩵", .name = "light blue heart", .group = GRP(0), .keywords = EKW_149, .skinToneSupport = false},
	EmojiData{ .emoji = "💜", .name = "purple heart", .group = GRP(0), .keywords = EKW_150, .skinToneSupport = false},
	EmojiData{ .emoji = "🤎", .name = "brown heart", .group = GRP(0), .keywords = EKW_151, .skinToneSupport = false},
	EmojiData{ .emoji = "🖤", .name = "black heart", .group = GRP(0), .keywords = EKW_152, .skinToneSupport = false},
	EmojiData{ .emoji = "🩶", .name = "grey heart", .group = GRP(0), .keywords = EKW_153, .skinToneSupport = false},
	EmojiData{ .emoji = "🤍", .name = "white heart", .group = GRP(0), .keywords = EKW_154, .skinToneSupport = false},
	EmojiData{ .emoji = "💋", .name = "kiss mark", .group = GRP(0), .keywords = EKW_155, .skinToneSupport = false},
	EmojiData{ .emoji = "💯", .name = "hundred points", .group = GRP(0), .keywords = EKW_156, .skinToneSupport = false},
	EmojiData{ .emoji = "💢", .name = "anger symbol", .group = GRP(0), .keywords = EKW_157, .skinToneSupport = false},
	EmojiData{ .emoji = "💥", .name = "collision", .group = GRP(0), .keywords = EKW_158, .skinToneSupport = false},
	EmojiData{ .emoji = "💫", .name = "dizzy", .group = GRP(0), .keywords = EKW_159, .skinToneSupport = false},
	EmojiData{ .emoji = "💦", .name = "sweat droplets", .group = GRP(0), .keywords = EKW_160, .skinToneSupport = false},
	EmojiData{ .emoji = "💨", .name = "dashing away", .group = GRP(0), .keywords = EKW_161, .skinToneSupport = false},
	EmojiData{ .emoji = "🕳️", .name = "hole", .group = GRP(0), .keywords = EKW_162, .skinToneSupport = false},
	EmojiData{ .emoji = "💬", .name = "speech balloon", .group = GRP(0), .keywords = EKW_163, .skinToneSupport = false},
	EmojiData{ .emoji = "👁️‍🗨️", .name = "eye in speech bubble", .group = GRP(0), .keywords = EKW_164, .skinToneSupport = false},
	EmojiData{ .emoji = "🗨️", .name = "left speech bubble", .group = GRP(0), .keywords = EKW_165, .skinToneSupport = false},
	EmojiData{ .emoji = "🗯️", .name = "right anger bubble", .group = GRP(0), .keywords = EKW_166, .skinToneSupport = false},
	EmojiData{ .emoji = "💭", .name = "thought balloon", .group = GRP(0), .keywords = EKW_167, .skinToneSupport = false},
	EmojiData{ .emoji = "💤", .name = "ZZZ", .group = GRP(0), .keywords = EKW_168, .skinToneSupport = false},
	EmojiData{ .emoji = "👋", .name = "waving hand", .group = GRP(1), .keywords = EKW_169, .skinToneSupport = true},
	EmojiData{ .emoji = "🤚", .name = "raised back of hand", .group = GRP(1), .keywords = EKW_170, .skinToneSupport = true},
	EmojiData{ .emoji = "🖐️", .name = "hand with fingers splayed", .group = GRP(1), .keywords = EKW_171, .skinToneSupport = true},
	EmojiData{ .emoji = "✋", .name = "raised hand", .group = GRP(1), .keywords = EKW_172, .skinToneSupport = true},
	EmojiData{ .emoji = "🖖", .name = "vulcan salute", .group = GRP(1), .keywords = EKW_173, .skinToneSupport = true},
	EmojiData{ .emoji = "🫱", .name = "rightwards hand", .group = GRP(1), .keywords = EKW_174, .skinToneSupport = true},
	EmojiData{ .emoji = "🫲", .name = "leftwards hand", .group = GRP(1), .keywords = EKW_175, .skinToneSupport = true},
	EmojiData{ .emoji = "🫳", .name = "palm down hand", .group = GRP(1), .keywords = EKW_176, .skinToneSupport = true},
	EmojiData{ .emoji = "🫴", .name = "palm up hand", .group = GRP(1), .keywords = EKW_177, .skinToneSupport = true},
	EmojiData{ .emoji = "🫷", .name = "leftwards pushing hand", .group = GRP(1), .keywords = EKW_178, .skinToneSupport = true},
	EmojiData{ .emoji = "🫸", .name = "rightwards pushing hand", .group = GRP(1), .keywords = EKW_179, .skinToneSupport = true},
	EmojiData{ .emoji = "👌", .name = "OK hand", .group = GRP(1), .keywords = EKW_180, .skinToneSupport = true},
	EmojiData{ .emoji = "🤌", .name = "pinched fingers", .group = GRP(1), .keywords = EKW_181, .skinToneSupport = true},
	EmojiData{ .emoji = "🤏", .name = "pinching hand", .group = GRP(1), .keywords = EKW_182, .skinToneSupport = true},
	EmojiData{ .emoji = "✌️", .name = "victory hand", .group = GRP(1), .keywords = EKW_183, .skinToneSupport = true},
	EmojiData{ .emoji = "🤞", .name = "crossed fingers", .group = GRP(1), .keywords = EKW_184, .skinToneSupport = true},
	EmojiData{ .emoji = "🫰", .name = "hand with index finger and thumb crossed", .group = GRP(1), .keywords = EKW_185, .skinToneSupport = true},
	EmojiData{ .emoji = "🤟", .name = "love-you gesture", .group = GRP(1), .keywords = EKW_186, .skinToneSupport = true},
	EmojiData{ .emoji = "🤘", .name = "sign of the horns", .group = GRP(1), .keywords = EKW_187, .skinToneSupport = true},
	EmojiData{ .emoji = "🤙", .name = "call me hand", .group = GRP(1), .keywords = EKW_188, .skinToneSupport = true},
	EmojiData{ .emoji = "👈", .name = "backhand index pointing left", .group = GRP(1), .keywords = EKW_189, .skinToneSupport = true},
	EmojiData{ .emoji = "👉", .name = "backhand index pointing right", .group = GRP(1), .keywords = EKW_190, .skinToneSupport = true},
	EmojiData{ .emoji = "👆", .name = "backhand index pointing up", .group = GRP(1), .keywords = EKW_191, .skinToneSupport = true},
	EmojiData{ .emoji = "🖕", .name = "middle finger", .group = GRP(1), .keywords = EKW_192, .skinToneSupport = true},
	EmojiData{ .emoji = "👇", .name = "backhand index pointing down", .group = GRP(1), .keywords = EKW_193, .skinToneSupport = true},
	EmojiData{ .emoji = "☝️", .name = "index pointing up", .group = GRP(1), .keywords = EKW_194, .skinToneSupport = true},
	EmojiData{ .emoji = "🫵", .name = "index pointing at the viewer", .group = GRP(1), .keywords = EKW_195, .skinToneSupport = true},
	EmojiData{ .emoji = "👍", .name = "thumbs up", .group = GRP(1), .keywords = EKW_196, .skinToneSupport = true},
	EmojiData{ .emoji = "👎", .name = "thumbs down", .group = GRP(1), .keywords = EKW_197, .skinToneSupport = true},
	EmojiData{ .emoji = "✊", .name = "raised fist", .group = GRP(1), .keywords = EKW_198, .skinToneSupport = true},
	EmojiData{ .emoji = "👊", .name = "oncoming fist", .group = GRP(1), .keywords = EKW_199, .skinToneSupport = true},
	EmojiData{ .emoji = "🤛", .name = "left-facing fist", .group = GRP(1), .keywords = EKW_200, .skinToneSupport = true},
	EmojiData{ .emoji = "🤜", .name = "right-facing fist", .group = GRP(1), .keywords = EKW_201, .skinToneSupport = true},
	EmojiData{ .emoji = "👏", .name = "clapping hands", .group = GRP(1), .keywords = EKW_202, .skinToneSupport = true},
	EmojiData{ .emoji = "🙌", .name = "raising hands", .group = GRP(1), .keywords = EKW_203, .skinToneSupport = true},
	EmojiData{ .emoji = "🫶", .name = "heart hands", .group = GRP(1), .keywords = EKW_204, .skinToneSupport = true},
	EmojiData{ .emoji = "👐", .name = "open hands", .group = GRP(1), .keywords = EKW_205, .skinToneSupport = true},
	EmojiData{ .emoji = "🤲", .name = "palms up together", .group = GRP(1), .keywords = EKW_206, .skinToneSupport = true},
	EmojiData{ .emoji = "🤝", .name = "handshake", .group = GRP(1), .keywords = EKW_207, .skinToneSupport = true},
	EmojiData{ .emoji = "🙏", .name = "folded hands", .group = GRP(1), .keywords = EKW_208, .skinToneSupport = true},
	EmojiData{ .emoji = "✍️", .name = "writing hand", .group = GRP(1), .keywords = EKW_209, .skinToneSupport = true},
	EmojiData{ .emoji = "💅", .name = "nail polish", .group = GRP(1), .keywords = EKW_210, .skinToneSupport = true},
	EmojiData{ .emoji = "🤳", .name = "selfie", .group = GRP(1), .keywords = EKW_211, .skinToneSupport = true},
	EmojiData{ .emoji = "💪", .name = "flexed biceps", .group = GRP(1), .keywords = EKW_212, .skinToneSupport = true},
	EmojiData{ .emoji = "🦾", .name = "mechanical arm", .group = GRP(1), .keywords = EKW_213, .skinToneSupport = false},
	EmojiData{ .emoji = "🦿", .name = "mechanical leg", .group = GRP(1), .keywords = EKW_214, .skinToneSupport = false},
	EmojiData{ .emoji = "🦵", .name = "leg", .group = GRP(1), .keywords = EKW_215, .skinToneSupport = true},
	EmojiData{ .emoji = "🦶", .name = "foot", .group = GRP(1), .keywords = EKW_216, .skinToneSupport = true},
	EmojiData{ .emoji = "👂", .name = "ear", .group = GRP(1), .keywords = EKW_217, .skinToneSupport = true},
	EmojiData{ .emoji = "🦻", .name = "ear with hearing aid", .group = GRP(1), .keywords = EKW_218, .skinToneSupport = true},
	EmojiData{ .emoji = "👃", .name = "nose", .group = GRP(1), .keywords = EKW_219, .skinToneSupport = true},
	EmojiData{ .emoji = "🧠", .name = "brain", .group = GRP(1), .keywords = EKW_220, .skinToneSupport = false},
	EmojiData{ .emoji = "🫀", .name = "anatomical heart", .group = GRP(1), .keywords = EKW_221, .skinToneSupport = false},
	EmojiData{ .emoji = "🫁", .name = "lungs", .group = GRP(1), .keywords = EKW_222, .skinToneSupport = false},
	EmojiData{ .emoji = "🦷", .name = "tooth", .group = GRP(1), .keywords = EKW_223, .skinToneSupport = false},
	EmojiData{ .emoji = "🦴", .name = "bone", .group = GRP(1), .keywords = EKW_224, .skinToneSupport = false},
	EmojiData{ .emoji = "👀", .name = "eyes", .group = GRP(1), .keywords = EKW_225, .skinToneSupport = false},
	EmojiData{ .emoji = "👁️", .name = "eye", .group = GRP(1), .keywords = EKW_226, .skinToneSupport = false},
	EmojiData{ .emoji = "👅", .name = "tongue", .group = GRP(1), .keywords = EKW_227, .skinToneSupport = false},
	EmojiData{ .emoji = "👄", .name = "mouth", .group = GRP(1), .keywords = EKW_228, .skinToneSupport = false},
	EmojiData{ .emoji = "🫦", .name = "biting lip", .group = GRP(1), .keywords = EKW_229, .skinToneSupport = false},
	EmojiData{ .emoji = "👶", .name = "baby", .group = GRP(1), .keywords = EKW_230, .skinToneSupport = true},
	EmojiData{ .emoji = "🧒", .name = "child", .group = GRP(1), .keywords = EKW_231, .skinToneSupport = true},
	EmojiData{ .emoji = "👦", .name = "boy", .group = GRP(1), .keywords = EKW_232, .skinToneSupport = true},
	EmojiData{ .emoji = "👧", .name = "girl", .group = GRP(1), .keywords = EKW_233, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑", .name = "person", .group = GRP(1), .keywords = EKW_234, .skinToneSupport = true},
	EmojiData{ .emoji = "👱", .name = "person blond hair", .group = GRP(1), .keywords = EKW_235, .skinToneSupport = true},
	EmojiData{ .emoji = "👨", .name = "man", .group = GRP(1), .keywords = EKW_236, .skinToneSupport = true},
	EmojiData{ .emoji = "🧔", .name = "person beard", .group = GRP(1), .keywords = EKW_237, .skinToneSupport = true},
	EmojiData{ .emoji = "🧔‍♂️", .name = "man beard", .group = GRP(1), .keywords = EKW_238, .skinToneSupport = true},
	EmojiData{ .emoji = "🧔‍♀️", .name = "woman beard", .group = GRP(1), .keywords = EKW_239, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦰", .name = "man red hair", .group = GRP(1), .keywords = EKW_240, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦱", .name = "man curly hair", .group = GRP(1), .keywords = EKW_241, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦳", .name = "man white hair", .group = GRP(1), .keywords = EKW_242, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦲", .name = "man bald", .group = GRP(1), .keywords = EKW_243, .skinToneSupport = true},
	EmojiData{ .emoji = "👩", .name = "woman", .group = GRP(1), .keywords = EKW_244, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦰", .name = "woman red hair", .group = GRP(1), .keywords = EKW_245, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦰", .name = "person red hair", .group = GRP(1), .keywords = EKW_246, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦱", .name = "woman curly hair", .group = GRP(1), .keywords = EKW_247, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦱", .name = "person curly hair", .group = GRP(1), .keywords = EKW_248, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦳", .name = "woman white hair", .group = GRP(1), .keywords = EKW_249, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦳", .name = "person white hair", .group = GRP(1), .keywords = EKW_250, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦲", .name = "woman bald", .group = GRP(1), .keywords = EKW_251, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦲", .name = "person bald", .group = GRP(1), .keywords = EKW_252, .skinToneSupport = true},
	EmojiData{ .emoji = "👱‍♀️", .name = "woman blond hair", .group = GRP(1), .keywords = EKW_253, .skinToneSupport = true},
	EmojiData{ .emoji = "👱‍♂️", .name = "man blond hair", .group = GRP(1), .keywords = EKW_254, .skinToneSupport = true},
	EmojiData{ .emoji = "🧓", .name = "older person", .group = GRP(1), .keywords = EKW_255, .skinToneSupport = true},
	EmojiData{ .emoji = "👴", .name = "old man", .group = GRP(1), .keywords = EKW_256, .skinToneSupport = true},
	EmojiData{ .emoji = "👵", .name = "old woman", .group = GRP(1), .keywords = EKW_257, .skinToneSupport = true},
	EmojiData{ .emoji = "🙍", .name = "person frowning", .group = GRP(1), .keywords = EKW_258, .skinToneSupport = true},
	EmojiData{ .emoji = "🙍‍♂️", .name = "man frowning", .group = GRP(1), .keywords = EKW_259, .skinToneSupport = true},
	EmojiData{ .emoji = "🙍‍♀️", .name = "woman frowning", .group = GRP(1), .keywords = EKW_260, .skinToneSupport = true},
	EmojiData{ .emoji = "🙎", .name = "person pouting", .group = GRP(1), .keywords = EKW_261, .skinToneSupport = true},
	EmojiData{ .emoji = "🙎‍♂️", .name = "man pouting", .group = GRP(1), .keywords = EKW_262, .skinToneSupport = true},
	EmojiData{ .emoji = "🙎‍♀️", .name = "woman pouting", .group = GRP(1), .keywords = EKW_263, .skinToneSupport = true},
	EmojiData{ .emoji = "🙅", .name = "person gesturing NO", .group = GRP(1), .keywords = EKW_264, .skinToneSupport = true},
	EmojiData{ .emoji = "🙅‍♂️", .name = "man gesturing NO", .group = GRP(1), .keywords = EKW_265, .skinToneSupport = true},
	EmojiData{ .emoji = "🙅‍♀️", .name = "woman gesturing NO", .group = GRP(1), .keywords = EKW_266, .skinToneSupport = true},
	EmojiData{ .emoji = "🙆", .name = "person gesturing OK", .group = GRP(1), .keywords = EKW_267, .skinToneSupport = true},
	EmojiData{ .emoji = "🙆‍♂️", .name = "man gesturing OK", .group = GRP(1), .keywords = EKW_268, .skinToneSupport = true},
	EmojiData{ .emoji = "🙆‍♀️", .name = "woman gesturing OK", .group = GRP(1), .keywords = EKW_269, .skinToneSupport = true},
	EmojiData{ .emoji = "💁", .name = "person tipping hand", .group = GRP(1), .keywords = EKW_270, .skinToneSupport = true},
	EmojiData{ .emoji = "💁‍♂️", .name = "man tipping hand", .group = GRP(1), .keywords = EKW_271, .skinToneSupport = true},
	EmojiData{ .emoji = "💁‍♀️", .name = "woman tipping hand", .group = GRP(1), .keywords = EKW_272, .skinToneSupport = true},
	EmojiData{ .emoji = "🙋", .name = "person raising hand", .group = GRP(1), .keywords = EKW_273, .skinToneSupport = true},
	EmojiData{ .emoji = "🙋‍♂️", .name = "man raising hand", .group = GRP(1), .keywords = EKW_274, .skinToneSupport = true},
	EmojiData{ .emoji = "🙋‍♀️", .name = "woman raising hand", .group = GRP(1), .keywords = EKW_275, .skinToneSupport = true},
	EmojiData{ .emoji = "🧏", .name = "deaf person", .group = GRP(1), .keywords = EKW_276, .skinToneSupport = true},
	EmojiData{ .emoji = "🧏‍♂️", .name = "deaf man", .group = GRP(1), .keywords = EKW_277, .skinToneSupport = true},
	EmojiData{ .emoji = "🧏‍♀️", .name = "deaf woman", .group = GRP(1), .keywords = EKW_278, .skinToneSupport = true},
	EmojiData{ .emoji = "🙇", .name = "person bowing", .group = GRP(1), .keywords = EKW_279, .skinToneSupport = true},
	EmojiData{ .emoji = "🙇‍♂️", .name = "man bowing", .group = GRP(1), .keywords = EKW_280, .skinToneSupport = true},
	EmojiData{ .emoji = "🙇‍♀️", .name = "woman bowing", .group = GRP(1), .keywords = EKW_281, .skinToneSupport = true},
	EmojiData{ .emoji = "🤦", .name = "person facepalming", .group = GRP(1), .keywords = EKW_282, .skinToneSupport = true},
	EmojiData{ .emoji = "🤦‍♂️", .name = "man facepalming", .group = GRP(1), .keywords = EKW_283, .skinToneSupport = true},
	EmojiData{ .emoji = "🤦‍♀️", .name = "woman facepalming", .group = GRP(1), .keywords = EKW_284, .skinToneSupport = true},
	EmojiData{ .emoji = "🤷", .name = "person shrugging", .group = GRP(1), .keywords = EKW_285, .skinToneSupport = true},
	EmojiData{ .emoji = "🤷‍♂️", .name = "man shrugging", .group = GRP(1), .keywords = EKW_286, .skinToneSupport = true},
	EmojiData{ .emoji = "🤷‍♀️", .name = "woman shrugging", .group = GRP(1), .keywords = EKW_287, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍⚕️", .name = "health worker", .group = GRP(1), .keywords = EKW_288, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍⚕️", .name = "man health worker", .group = GRP(1), .keywords = EKW_289, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍⚕️", .name = "woman health worker", .group = GRP(1), .keywords = EKW_290, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🎓", .name = "student", .group = GRP(1), .keywords = EKW_291, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🎓", .name = "man student", .group = GRP(1), .keywords = EKW_292, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🎓", .name = "woman student", .group = GRP(1), .keywords = EKW_293, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🏫", .name = "teacher", .group = GRP(1), .keywords = EKW_294, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🏫", .name = "man teacher", .group = GRP(1), .keywords = EKW_295, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🏫", .name = "woman teacher", .group = GRP(1), .keywords = EKW_296, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍⚖️", .name = "judge", .group = GRP(1), .keywords = EKW_297, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍⚖️", .name = "man judge", .group = GRP(1), .keywords = EKW_298, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍⚖️", .name = "woman judge", .group = GRP(1), .keywords = EKW_299, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🌾", .name = "farmer", .group = GRP(1), .keywords = EKW_300, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🌾", .name = "man farmer", .group = GRP(1), .keywords = EKW_301, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🌾", .name = "woman farmer", .group = GRP(1), .keywords = EKW_302, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🍳", .name = "cook", .group = GRP(1), .keywords = EKW_303, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🍳", .name = "man cook", .group = GRP(1), .keywords = EKW_304, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🍳", .name = "woman cook", .group = GRP(1), .keywords = EKW_305, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🔧", .name = "mechanic", .group = GRP(1), .keywords = EKW_306, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🔧", .name = "man mechanic", .group = GRP(1), .keywords = EKW_307, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🔧", .name = "woman mechanic", .group = GRP(1), .keywords = EKW_308, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🏭", .name = "factory worker", .group = GRP(1), .keywords = EKW_309, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🏭", .name = "man factory worker", .group = GRP(1), .keywords = EKW_310, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🏭", .name = "woman factory worker", .group = GRP(1), .keywords = EKW_311, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍💼", .name = "office worker", .group = GRP(1), .keywords = EKW_312, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍💼", .name = "man office worker", .group = GRP(1), .keywords = EKW_313, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍💼", .name = "woman office worker", .group = GRP(1), .keywords = EKW_314, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🔬", .name = "scientist", .group = GRP(1), .keywords = EKW_315, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🔬", .name = "man scientist", .group = GRP(1), .keywords = EKW_316, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🔬", .name = "woman scientist", .group = GRP(1), .keywords = EKW_317, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍💻", .name = "technologist", .group = GRP(1), .keywords = EKW_318, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍💻", .name = "man technologist", .group = GRP(1), .keywords = EKW_319, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍💻", .name = "woman technologist", .group = GRP(1), .keywords = EKW_320, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🎤", .name = "singer", .group = GRP(1), .keywords = EKW_321, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🎤", .name = "man singer", .group = GRP(1), .keywords = EKW_322, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🎤", .name = "woman singer", .group = GRP(1), .keywords = EKW_323, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🎨", .name = "artist", .group = GRP(1), .keywords = EKW_324, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🎨", .name = "man artist", .group = GRP(1), .keywords = EKW_325, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🎨", .name = "woman artist", .group = GRP(1), .keywords = EKW_326, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍✈️", .name = "pilot", .group = GRP(1), .keywords = EKW_327, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍✈️", .name = "man pilot", .group = GRP(1), .keywords = EKW_328, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍✈️", .name = "woman pilot", .group = GRP(1), .keywords = EKW_329, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🚀", .name = "astronaut", .group = GRP(1), .keywords = EKW_330, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🚀", .name = "man astronaut", .group = GRP(1), .keywords = EKW_331, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🚀", .name = "woman astronaut", .group = GRP(1), .keywords = EKW_332, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🚒", .name = "firefighter", .group = GRP(1), .keywords = EKW_333, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🚒", .name = "man firefighter", .group = GRP(1), .keywords = EKW_334, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🚒", .name = "woman firefighter", .group = GRP(1), .keywords = EKW_335, .skinToneSupport = true},
	EmojiData{ .emoji = "👮", .name = "police officer", .group = GRP(1), .keywords = EKW_336, .skinToneSupport = true},
	EmojiData{ .emoji = "👮‍♂️", .name = "man police officer", .group = GRP(1), .keywords = EKW_337, .skinToneSupport = true},
	EmojiData{ .emoji = "👮‍♀️", .name = "woman police officer", .group = GRP(1), .keywords = EKW_338, .skinToneSupport = true},
	EmojiData{ .emoji = "🕵️", .name = "detective", .group = GRP(1), .keywords = EKW_339, .skinToneSupport = true},
	EmojiData{ .emoji = "🕵️‍♂️", .name = "man detective", .group = GRP(1), .keywords = EKW_340, .skinToneSupport = true},
	EmojiData{ .emoji = "🕵️‍♀️", .name = "woman detective", .group = GRP(1), .keywords = EKW_341, .skinToneSupport = true},
	EmojiData{ .emoji = "💂", .name = "guard", .group = GRP(1), .keywords = EKW_342, .skinToneSupport = true},
	EmojiData{ .emoji = "💂‍♂️", .name = "man guard", .group = GRP(1), .keywords = EKW_343, .skinToneSupport = true},
	EmojiData{ .emoji = "💂‍♀️", .name = "woman guard", .group = GRP(1), .keywords = EKW_344, .skinToneSupport = true},
	EmojiData{ .emoji = "🥷", .name = "ninja", .group = GRP(1), .keywords = EKW_345, .skinToneSupport = true},
	EmojiData{ .emoji = "👷", .name = "construction worker", .group = GRP(1), .keywords = EKW_346, .skinToneSupport = true},
	EmojiData{ .emoji = "👷‍♂️", .name = "man construction worker", .group = GRP(1), .keywords = EKW_347, .skinToneSupport = true},
	EmojiData{ .emoji = "👷‍♀️", .name = "woman construction worker", .group = GRP(1), .keywords = EKW_348, .skinToneSupport = true},
	EmojiData{ .emoji = "🫅", .name = "person with crown", .group = GRP(1), .keywords = EKW_349, .skinToneSupport = true},
	EmojiData{ .emoji = "🤴", .name = "prince", .group = GRP(1), .keywords = EKW_350, .skinToneSupport = true},
	EmojiData{ .emoji = "👸", .name = "princess", .group = GRP(1), .keywords = EKW_351, .skinToneSupport = true},
	EmojiData{ .emoji = "👳", .name = "person wearing turban", .group = GRP(1), .keywords = EKW_352, .skinToneSupport = true},
	EmojiData{ .emoji = "👳‍♂️", .name = "man wearing turban", .group = GRP(1), .keywords = EKW_353, .skinToneSupport = true},
	EmojiData{ .emoji = "👳‍♀️", .name = "woman wearing turban", .group = GRP(1), .keywords = EKW_354, .skinToneSupport = true},
	EmojiData{ .emoji = "👲", .name = "person with skullcap", .group = GRP(1), .keywords = EKW_355, .skinToneSupport = true},
	EmojiData{ .emoji = "🧕", .name = "woman with headscarf", .group = GRP(1), .keywords = EKW_356, .skinToneSupport = true},
	EmojiData{ .emoji = "🤵", .name = "person in tuxedo", .group = GRP(1), .keywords = EKW_357, .skinToneSupport = true},
	EmojiData{ .emoji = "🤵‍♂️", .name = "man in tuxedo", .group = GRP(1), .keywords = EKW_358, .skinToneSupport = true},
	EmojiData{ .emoji = "🤵‍♀️", .name = "woman in tuxedo", .group = GRP(1), .keywords = EKW_359, .skinToneSupport = true},
	EmojiData{ .emoji = "👰", .name = "person with veil", .group = GRP(1), .keywords = EKW_360, .skinToneSupport = true},
	EmojiData{ .emoji = "👰‍♂️", .name = "man with veil", .group = GRP(1), .keywords = EKW_361, .skinToneSupport = true},
	EmojiData{ .emoji = "👰‍♀️", .name = "woman with veil", .group = GRP(1), .keywords = EKW_362, .skinToneSupport = true},
	EmojiData{ .emoji = "🤰", .name = "pregnant woman", .group = GRP(1), .keywords = EKW_363, .skinToneSupport = true},
	EmojiData{ .emoji = "🫃", .name = "pregnant man", .group = GRP(1), .keywords = EKW_364, .skinToneSupport = true},
	EmojiData{ .emoji = "🫄", .name = "pregnant person", .group = GRP(1), .keywords = EKW_365, .skinToneSupport = true},
	EmojiData{ .emoji = "🤱", .name = "breast-feeding", .group = GRP(1), .keywords = EKW_366, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🍼", .name = "woman feeding baby", .group = GRP(1), .keywords = EKW_367, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🍼", .name = "man feeding baby", .group = GRP(1), .keywords = EKW_368, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🍼", .name = "person feeding baby", .group = GRP(1), .keywords = EKW_369, .skinToneSupport = true},
	EmojiData{ .emoji = "👼", .name = "baby angel", .group = GRP(1), .keywords = EKW_370, .skinToneSupport = true},
	EmojiData{ .emoji = "🎅", .name = "Santa Claus", .group = GRP(1), .keywords = EKW_371, .skinToneSupport = true},
	EmojiData{ .emoji = "🤶", .name = "Mrs. Claus", .group = GRP(1), .keywords = EKW_372, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🎄", .name = "Mx Claus", .group = GRP(1), .keywords = EKW_373, .skinToneSupport = true},
	EmojiData{ .emoji = "🦸", .name = "superhero", .group = GRP(1), .keywords = EKW_374, .skinToneSupport = true},
	EmojiData{ .emoji = "🦸‍♂️", .name = "man superhero", .group = GRP(1), .keywords = EKW_375, .skinToneSupport = true},
	EmojiData{ .emoji = "🦸‍♀️", .name = "woman superhero", .group = GRP(1), .keywords = EKW_376, .skinToneSupport = true},
	EmojiData{ .emoji = "🦹", .name = "supervillain", .group = GRP(1), .keywords = EKW_377, .skinToneSupport = true},
	EmojiData{ .emoji = "🦹‍♂️", .name = "man supervillain", .group = GRP(1), .keywords = EKW_378, .skinToneSupport = true},
	EmojiData{ .emoji = "🦹‍♀️", .name = "woman supervillain", .group = GRP(1), .keywords = EKW_379, .skinToneSupport = true},
	EmojiData{ .emoji = "🧙", .name = "mage", .group = GRP(1), .keywords = EKW_380, .skinToneSupport = true},
	EmojiData{ .emoji = "🧙‍♂️", .name = "man mage", .group = GRP(1), .keywords = EKW_381, .skinToneSupport = true},
	EmojiData{ .emoji = "🧙‍♀️", .name = "woman mage", .group = GRP(1), .keywords = EKW_382, .skinToneSupport = true},
	EmojiData{ .emoji = "🧚", .name = "fairy", .group = GRP(1), .keywords = EKW_383, .skinToneSupport = true},
	EmojiData{ .emoji = "🧚‍♂️", .name = "man fairy", .group = GRP(1), .keywords = EKW_384, .skinToneSupport = true},
	EmojiData{ .emoji = "🧚‍♀️", .name = "woman fairy", .group = GRP(1), .keywords = EKW_385, .skinToneSupport = true},
	EmojiData{ .emoji = "🧛", .name = "vampire", .group = GRP(1), .keywords = EKW_386, .skinToneSupport = true},
	EmojiData{ .emoji = "🧛‍♂️", .name = "man vampire", .group = GRP(1), .keywords = EKW_387, .skinToneSupport = true},
	EmojiData{ .emoji = "🧛‍♀️", .name = "woman vampire", .group = GRP(1), .keywords = EKW_388, .skinToneSupport = true},
	EmojiData{ .emoji = "🧜", .name = "merperson", .group = GRP(1), .keywords = EKW_389, .skinToneSupport = true},
	EmojiData{ .emoji = "🧜‍♂️", .name = "merman", .group = GRP(1), .keywords = EKW_390, .skinToneSupport = true},
	EmojiData{ .emoji = "🧜‍♀️", .name = "mermaid", .group = GRP(1), .keywords = EKW_391, .skinToneSupport = true},
	EmojiData{ .emoji = "🧝", .name = "elf", .group = GRP(1), .keywords = EKW_392, .skinToneSupport = true},
	EmojiData{ .emoji = "🧝‍♂️", .name = "man elf", .group = GRP(1), .keywords = EKW_393, .skinToneSupport = true},
	EmojiData{ .emoji = "🧝‍♀️", .name = "woman elf", .group = GRP(1), .keywords = EKW_394, .skinToneSupport = true},
	EmojiData{ .emoji = "🧞", .name = "genie", .group = GRP(1), .keywords = EKW_395, .skinToneSupport = false},
	EmojiData{ .emoji = "🧞‍♂️", .name = "man genie", .group = GRP(1), .keywords = EKW_396, .skinToneSupport = false},
	EmojiData{ .emoji = "🧞‍♀️", .name = "woman genie", .group = GRP(1), .keywords = EKW_397, .skinToneSupport = false},
	EmojiData{ .emoji = "🧟", .name = "zombie", .group = GRP(1), .keywords = EKW_398, .skinToneSupport = false},
	EmojiData{ .emoji = "🧟‍♂️", .name = "man zombie", .group = GRP(1), .keywords = EKW_399, .skinToneSupport = false},
	EmojiData{ .emoji = "🧟‍♀️", .name = "woman zombie", .group = GRP(1), .keywords = EKW_400, .skinToneSupport = false},
	EmojiData{ .emoji = "🧌", .name = "troll", .group = GRP(1), .keywords = EKW_401, .skinToneSupport = false},
	EmojiData{ .emoji = "💆", .name = "person getting massage", .group = GRP(1), .keywords = EKW_402, .skinToneSupport = true},
	EmojiData{ .emoji = "💆‍♂️", .name = "man getting massage", .group = GRP(1), .keywords = EKW_403, .skinToneSupport = true},
	EmojiData{ .emoji = "💆‍♀️", .name = "woman getting massage", .group = GRP(1), .keywords = EKW_404, .skinToneSupport = true},
	EmojiData{ .emoji = "💇", .name = "person getting haircut", .group = GRP(1), .keywords = EKW_405, .skinToneSupport = true},
	EmojiData{ .emoji = "💇‍♂️", .name = "man getting haircut", .group = GRP(1), .keywords = EKW_406, .skinToneSupport = true},
	EmojiData{ .emoji = "💇‍♀️", .name = "woman getting haircut", .group = GRP(1), .keywords = EKW_407, .skinToneSupport = true},
	EmojiData{ .emoji = "🚶", .name = "person walking", .group = GRP(1), .keywords = EKW_408, .skinToneSupport = true},
	EmojiData{ .emoji = "🚶‍♂️", .name = "man walking", .group = GRP(1), .keywords = EKW_409, .skinToneSupport = true},
	EmojiData{ .emoji = "🚶‍♀️", .name = "woman walking", .group = GRP(1), .keywords = EKW_410, .skinToneSupport = true},
	EmojiData{ .emoji = "🚶‍➡️", .name = "person walking facing right", .group = GRP(1), .keywords = EKW_411, .skinToneSupport = true},
	EmojiData{ .emoji = "🚶‍♀️‍➡️", .name = "woman walking facing right", .group = GRP(1), .keywords = EKW_412, .skinToneSupport = true},
	EmojiData{ .emoji = "🚶‍♂️‍➡️", .name = "man walking facing right", .group = GRP(1), .keywords = EKW_413, .skinToneSupport = true},
	EmojiData{ .emoji = "🧍", .name = "person standing", .group = GRP(1), .keywords = EKW_414, .skinToneSupport = true},
	EmojiData{ .emoji = "🧍‍♂️", .name = "man standing", .group = GRP(1), .keywords = EKW_415, .skinToneSupport = true},
	EmojiData{ .emoji = "🧍‍♀️", .name = "woman standing", .group = GRP(1), .keywords = EKW_416, .skinToneSupport = true},
	EmojiData{ .emoji = "🧎", .name = "person kneeling", .group = GRP(1), .keywords = EKW_417, .skinToneSupport = true},
	EmojiData{ .emoji = "🧎‍♂️", .name = "man kneeling", .group = GRP(1), .keywords = EKW_418, .skinToneSupport = true},
	EmojiData{ .emoji = "🧎‍♀️", .name = "woman kneeling", .group = GRP(1), .keywords = EKW_419, .skinToneSupport = true},
	EmojiData{ .emoji = "🧎‍➡️", .name = "person kneeling facing right", .group = GRP(1), .keywords = EKW_420, .skinToneSupport = true},
	EmojiData{ .emoji = "🧎‍♀️‍➡️", .name = "woman kneeling facing right", .group = GRP(1), .keywords = EKW_421, .skinToneSupport = true},
	EmojiData{ .emoji = "🧎‍♂️‍➡️", .name = "man kneeling facing right", .group = GRP(1), .keywords = EKW_422, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦯", .name = "person with white cane", .group = GRP(1), .keywords = EKW_423, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦯‍➡️", .name = "person with white cane facing right", .group = GRP(1), .keywords = EKW_424, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦯", .name = "man with white cane", .group = GRP(1), .keywords = EKW_425, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦯‍➡️", .name = "man with white cane facing right", .group = GRP(1), .keywords = EKW_426, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦯", .name = "woman with white cane", .group = GRP(1), .keywords = EKW_427, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦯‍➡️", .name = "woman with white cane facing right", .group = GRP(1), .keywords = EKW_428, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦼", .name = "person in motorized wheelchair", .group = GRP(1), .keywords = EKW_429, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦼‍➡️", .name = "person in motorized wheelchair facing right", .group = GRP(1), .keywords = EKW_430, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦼", .name = "man in motorized wheelchair", .group = GRP(1), .keywords = EKW_431, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦼‍➡️", .name = "man in motorized wheelchair facing right", .group = GRP(1), .keywords = EKW_432, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦼", .name = "woman in motorized wheelchair", .group = GRP(1), .keywords = EKW_433, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦼‍➡️", .name = "woman in motorized wheelchair facing right", .group = GRP(1), .keywords = EKW_434, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦽", .name = "person in manual wheelchair", .group = GRP(1), .keywords = EKW_435, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🦽‍➡️", .name = "person in manual wheelchair facing right", .group = GRP(1), .keywords = EKW_436, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦽", .name = "man in manual wheelchair", .group = GRP(1), .keywords = EKW_437, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍🦽‍➡️", .name = "man in manual wheelchair facing right", .group = GRP(1), .keywords = EKW_438, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦽", .name = "woman in manual wheelchair", .group = GRP(1), .keywords = EKW_439, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍🦽‍➡️", .name = "woman in manual wheelchair facing right", .group = GRP(1), .keywords = EKW_440, .skinToneSupport = true},
	EmojiData{ .emoji = "🏃", .name = "person running", .group = GRP(1), .keywords = EKW_441, .skinToneSupport = true},
	EmojiData{ .emoji = "🏃‍♂️", .name = "man running", .group = GRP(1), .keywords = EKW_442, .skinToneSupport = true},
	EmojiData{ .emoji = "🏃‍♀️", .name = "woman running", .group = GRP(1), .keywords = EKW_443, .skinToneSupport = true},
	EmojiData{ .emoji = "🏃‍➡️", .name = "person running facing right", .group = GRP(1), .keywords = EKW_444, .skinToneSupport = true},
	EmojiData{ .emoji = "🏃‍♀️‍➡️", .name = "woman running facing right", .group = GRP(1), .keywords = EKW_445, .skinToneSupport = true},
	EmojiData{ .emoji = "🏃‍♂️‍➡️", .name = "man running facing right", .group = GRP(1), .keywords = EKW_446, .skinToneSupport = true},
	EmojiData{ .emoji = "💃", .name = "woman dancing", .group = GRP(1), .keywords = EKW_447, .skinToneSupport = true},
	EmojiData{ .emoji = "🕺", .name = "man dancing", .group = GRP(1), .keywords = EKW_448, .skinToneSupport = true},
	EmojiData{ .emoji = "🕴️", .name = "person in suit levitating", .group = GRP(1), .keywords = EKW_449, .skinToneSupport = true},
	EmojiData{ .emoji = "👯", .name = "people with bunny ears", .group = GRP(1), .keywords = EKW_450, .skinToneSupport = false},
	EmojiData{ .emoji = "👯‍♂️", .name = "men with bunny ears", .group = GRP(1), .keywords = EKW_451, .skinToneSupport = false},
	EmojiData{ .emoji = "👯‍♀️", .name = "women with bunny ears", .group = GRP(1), .keywords = EKW_452, .skinToneSupport = false},
	EmojiData{ .emoji = "🧖", .name = "person in steamy room", .group = GRP(1), .keywords = EKW_453, .skinToneSupport = true},
	EmojiData{ .emoji = "🧖‍♂️", .name = "man in steamy room", .group = GRP(1), .keywords = EKW_454, .skinToneSupport = true},
	EmojiData{ .emoji = "🧖‍♀️", .name = "woman in steamy room", .group = GRP(1), .keywords = EKW_455, .skinToneSupport = true},
	EmojiData{ .emoji = "🧗", .name = "person climbing", .group = GRP(1), .keywords = EKW_456, .skinToneSupport = true},
	EmojiData{ .emoji = "🧗‍♂️", .name = "man climbing", .group = GRP(1), .keywords = EKW_457, .skinToneSupport = true},
	EmojiData{ .emoji = "🧗‍♀️", .name = "woman climbing", .group = GRP(1), .keywords = EKW_458, .skinToneSupport = true},
	EmojiData{ .emoji = "🤺", .name = "person fencing", .group = GRP(1), .keywords = EKW_459, .skinToneSupport = false},
	EmojiData{ .emoji = "🏇", .name = "horse racing", .group = GRP(1), .keywords = EKW_460, .skinToneSupport = true},
	EmojiData{ .emoji = "⛷️", .name = "skier", .group = GRP(1), .keywords = EKW_461, .skinToneSupport = false},
	EmojiData{ .emoji = "🏂", .name = "snowboarder", .group = GRP(1), .keywords = EKW_462, .skinToneSupport = true},
	EmojiData{ .emoji = "🏌️", .name = "person golfing", .group = GRP(1), .keywords = EKW_463, .skinToneSupport = true},
	EmojiData{ .emoji = "🏌️‍♂️", .name = "man golfing", .group = GRP(1), .keywords = EKW_464, .skinToneSupport = true},
	EmojiData{ .emoji = "🏌️‍♀️", .name = "woman golfing", .group = GRP(1), .keywords = EKW_465, .skinToneSupport = true},
	EmojiData{ .emoji = "🏄", .name = "person surfing", .group = GRP(1), .keywords = EKW_466, .skinToneSupport = true},
	EmojiData{ .emoji = "🏄‍♂️", .name = "man surfing", .group = GRP(1), .keywords = EKW_467, .skinToneSupport = true},
	EmojiData{ .emoji = "🏄‍♀️", .name = "woman surfing", .group = GRP(1), .keywords = EKW_468, .skinToneSupport = true},
	EmojiData{ .emoji = "🚣", .name = "person rowing boat", .group = GRP(1), .keywords = EKW_469, .skinToneSupport = true},
	EmojiData{ .emoji = "🚣‍♂️", .name = "man rowing boat", .group = GRP(1), .keywords = EKW_470, .skinToneSupport = true},
	EmojiData{ .emoji = "🚣‍♀️", .name = "woman rowing boat", .group = GRP(1), .keywords = EKW_471, .skinToneSupport = true},
	EmojiData{ .emoji = "🏊", .name = "person swimming", .group = GRP(1), .keywords = EKW_472, .skinToneSupport = true},
	EmojiData{ .emoji = "🏊‍♂️", .name = "man swimming", .group = GRP(1), .keywords = EKW_473, .skinToneSupport = true},
	EmojiData{ .emoji = "🏊‍♀️", .name = "woman swimming", .group = GRP(1), .keywords = EKW_474, .skinToneSupport = true},
	EmojiData{ .emoji = "⛹️", .name = "person bouncing ball", .group = GRP(1), .keywords = EKW_475, .skinToneSupport = true},
	EmojiData{ .emoji = "⛹️‍♂️", .name = "man bouncing ball", .group = GRP(1), .keywords = EKW_476, .skinToneSupport = true},
	EmojiData{ .emoji = "⛹️‍♀️", .name = "woman bouncing ball", .group = GRP(1), .keywords = EKW_477, .skinToneSupport = true},
	EmojiData{ .emoji = "🏋️", .name = "person lifting weights", .group = GRP(1), .keywords = EKW_478, .skinToneSupport = true},
	EmojiData{ .emoji = "🏋️‍♂️", .name = "man lifting weights", .group = GRP(1), .keywords = EKW_479, .skinToneSupport = true},
	EmojiData{ .emoji = "🏋️‍♀️", .name = "woman lifting weights", .group = GRP(1), .keywords = EKW_480, .skinToneSupport = true},
	EmojiData{ .emoji = "🚴", .name = "person biking", .group = GRP(1), .keywords = EKW_481, .skinToneSupport = true},
	EmojiData{ .emoji = "🚴‍♂️", .name = "man biking", .group = GRP(1), .keywords = EKW_482, .skinToneSupport = true},
	EmojiData{ .emoji = "🚴‍♀️", .name = "woman biking", .group = GRP(1), .keywords = EKW_483, .skinToneSupport = true},
	EmojiData{ .emoji = "🚵", .name = "person mountain biking", .group = GRP(1), .keywords = EKW_484, .skinToneSupport = true},
	EmojiData{ .emoji = "🚵‍♂️", .name = "man mountain biking", .group = GRP(1), .keywords = EKW_485, .skinToneSupport = true},
	EmojiData{ .emoji = "🚵‍♀️", .name = "woman mountain biking", .group = GRP(1), .keywords = EKW_486, .skinToneSupport = true},
	EmojiData{ .emoji = "🤸", .name = "person cartwheeling", .group = GRP(1), .keywords = EKW_487, .skinToneSupport = true},
	EmojiData{ .emoji = "🤸‍♂️", .name = "man cartwheeling", .group = GRP(1), .keywords = EKW_488, .skinToneSupport = true},
	EmojiData{ .emoji = "🤸‍♀️", .name = "woman cartwheeling", .group = GRP(1), .keywords = EKW_489, .skinToneSupport = true},
	EmojiData{ .emoji = "🤼", .name = "people wrestling", .group = GRP(1), .keywords = EKW_490, .skinToneSupport = false},
	EmojiData{ .emoji = "🤼‍♂️", .name = "men wrestling", .group = GRP(1), .keywords = EKW_491, .skinToneSupport = false},
	EmojiData{ .emoji = "🤼‍♀️", .name = "women wrestling", .group = GRP(1), .keywords = EKW_492, .skinToneSupport = false},
	EmojiData{ .emoji = "🤽", .name = "person playing water polo", .group = GRP(1), .keywords = EKW_493, .skinToneSupport = true},
	EmojiData{ .emoji = "🤽‍♂️", .name = "man playing water polo", .group = GRP(1), .keywords = EKW_494, .skinToneSupport = true},
	EmojiData{ .emoji = "🤽‍♀️", .name = "woman playing water polo", .group = GRP(1), .keywords = EKW_495, .skinToneSupport = true},
	EmojiData{ .emoji = "🤾", .name = "person playing handball", .group = GRP(1), .keywords = EKW_496, .skinToneSupport = true},
	EmojiData{ .emoji = "🤾‍♂️", .name = "man playing handball", .group = GRP(1), .keywords = EKW_497, .skinToneSupport = true},
	EmojiData{ .emoji = "🤾‍♀️", .name = "woman playing handball", .group = GRP(1), .keywords = EKW_498, .skinToneSupport = true},
	EmojiData{ .emoji = "🤹", .name = "person juggling", .group = GRP(1), .keywords = EKW_499, .skinToneSupport = true},
	EmojiData{ .emoji = "🤹‍♂️", .name = "man juggling", .group = GRP(1), .keywords = EKW_500, .skinToneSupport = true},
	EmojiData{ .emoji = "🤹‍♀️", .name = "woman juggling", .group = GRP(1), .keywords = EKW_501, .skinToneSupport = true},
	EmojiData{ .emoji = "🧘", .name = "person in lotus position", .group = GRP(1), .keywords = EKW_502, .skinToneSupport = true},
	EmojiData{ .emoji = "🧘‍♂️", .name = "man in lotus position", .group = GRP(1), .keywords = EKW_503, .skinToneSupport = true},
	EmojiData{ .emoji = "🧘‍♀️", .name = "woman in lotus position", .group = GRP(1), .keywords = EKW_504, .skinToneSupport = true},
	EmojiData{ .emoji = "🛀", .name = "person taking bath", .group = GRP(1), .keywords = EKW_505, .skinToneSupport = true},
	EmojiData{ .emoji = "🛌", .name = "person in bed", .group = GRP(1), .keywords = EKW_506, .skinToneSupport = true},
	EmojiData{ .emoji = "🧑‍🤝‍🧑", .name = "people holding hands", .group = GRP(1), .keywords = EKW_507, .skinToneSupport = true},
	EmojiData{ .emoji = "👭", .name = "women holding hands", .group = GRP(1), .keywords = EKW_508, .skinToneSupport = true},
	EmojiData{ .emoji = "👫", .name = "woman and man holding hands", .group = GRP(1), .keywords = EKW_509, .skinToneSupport = true},
	EmojiData{ .emoji = "👬", .name = "men holding hands", .group = GRP(1), .keywords = EKW_510, .skinToneSupport = true},
	EmojiData{ .emoji = "💏", .name = "kiss", .group = GRP(1), .keywords = EKW_511, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍❤️‍💋‍👨", .name = "kiss woman, man", .group = GRP(1), .keywords = EKW_512, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍❤️‍💋‍👨", .name = "kiss man, man", .group = GRP(1), .keywords = EKW_513, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍❤️‍💋‍👩", .name = "kiss woman, woman", .group = GRP(1), .keywords = EKW_514, .skinToneSupport = true},
	EmojiData{ .emoji = "💑", .name = "couple with heart", .group = GRP(1), .keywords = EKW_515, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍❤️‍👨", .name = "couple with heart woman, man", .group = GRP(1), .keywords = EKW_516, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍❤️‍👨", .name = "couple with heart man, man", .group = GRP(1), .keywords = EKW_517, .skinToneSupport = true},
	EmojiData{ .emoji = "👩‍❤️‍👩", .name = "couple with heart woman, woman", .group = GRP(1), .keywords = EKW_518, .skinToneSupport = true},
	EmojiData{ .emoji = "👨‍👩‍👦", .name = "family man, woman, boy", .group = GRP(1), .keywords = EKW_519, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👩‍👧", .name = "family man, woman, girl", .group = GRP(1), .keywords = EKW_520, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👩‍👧‍👦", .name = "family man, woman, girl, boy", .group = GRP(1), .keywords = EKW_521, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👩‍👦‍👦", .name = "family man, woman, boy, boy", .group = GRP(1), .keywords = EKW_522, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👩‍👧‍👧", .name = "family man, woman, girl, girl", .group = GRP(1), .keywords = EKW_523, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👨‍👦", .name = "family man, man, boy", .group = GRP(1), .keywords = EKW_524, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👨‍👧", .name = "family man, man, girl", .group = GRP(1), .keywords = EKW_525, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👨‍👧‍👦", .name = "family man, man, girl, boy", .group = GRP(1), .keywords = EKW_526, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👨‍👦‍👦", .name = "family man, man, boy, boy", .group = GRP(1), .keywords = EKW_527, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👨‍👧‍👧", .name = "family man, man, girl, girl", .group = GRP(1), .keywords = EKW_528, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👩‍👦", .name = "family woman, woman, boy", .group = GRP(1), .keywords = EKW_529, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👩‍👧", .name = "family woman, woman, girl", .group = GRP(1), .keywords = EKW_530, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👩‍👧‍👦", .name = "family woman, woman, girl, boy", .group = GRP(1), .keywords = EKW_531, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👩‍👦‍👦", .name = "family woman, woman, boy, boy", .group = GRP(1), .keywords = EKW_532, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👩‍👧‍👧", .name = "family woman, woman, girl, girl", .group = GRP(1), .keywords = EKW_533, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👦", .name = "family man, boy", .group = GRP(1), .keywords = EKW_534, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👦‍👦", .name = "family man, boy, boy", .group = GRP(1), .keywords = EKW_535, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👧", .name = "family man, girl", .group = GRP(1), .keywords = EKW_536, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👧‍👦", .name = "family man, girl, boy", .group = GRP(1), .keywords = EKW_537, .skinToneSupport = false},
	EmojiData{ .emoji = "👨‍👧‍👧", .name = "family man, girl, girl", .group = GRP(1), .keywords = EKW_538, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👦", .name = "family woman, boy", .group = GRP(1), .keywords = EKW_539, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👦‍👦", .name = "family woman, boy, boy", .group = GRP(1), .keywords = EKW_540, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👧", .name = "family woman, girl", .group = GRP(1), .keywords = EKW_541, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👧‍👦", .name = "family woman, girl, boy", .group = GRP(1), .keywords = EKW_542, .skinToneSupport = false},
	EmojiData{ .emoji = "👩‍👧‍👧", .name = "family woman, girl, girl", .group = GRP(1), .keywords = EKW_543, .skinToneSupport = false},
	EmojiData{ .emoji = "🗣️", .name = "speaking head", .group = GRP(1), .keywords = EKW_544, .skinToneSupport = false},
	EmojiData{ .emoji = "👤", .name = "bust in silhouette", .group = GRP(1), .keywords = EKW_545, .skinToneSupport = false},
	EmojiData{ .emoji = "👥", .name = "busts in silhouette", .group = GRP(1), .keywords = EKW_546, .skinToneSupport = false},
	EmojiData{ .emoji = "🫂", .name = "people hugging", .group = GRP(1), .keywords = EKW_547, .skinToneSupport = false},
	EmojiData{ .emoji = "👪", .name = "family", .group = GRP(1), .keywords = EKW_548, .skinToneSupport = false},
	EmojiData{ .emoji = "🧑‍🧑‍🧒", .name = "family adult, adult, child", .group = GRP(1), .keywords = EKW_549, .skinToneSupport = false},
	EmojiData{ .emoji = "🧑‍🧑‍🧒‍🧒", .name = "family adult, adult, child, child", .group = GRP(1), .keywords = EKW_550, .skinToneSupport = false},
	EmojiData{ .emoji = "🧑‍🧒", .name = "family adult, child", .group = GRP(1), .keywords = EKW_551, .skinToneSupport = false},
	EmojiData{ .emoji = "🧑‍🧒‍🧒", .name = "family adult, child, child", .group = GRP(1), .keywords = EKW_552, .skinToneSupport = false},
	EmojiData{ .emoji = "👣", .name = "footprints", .group = GRP(1), .keywords = EKW_553, .skinToneSupport = false},
	EmojiData{ .emoji = "🫆", .name = "fingerprint", .group = GRP(1), .keywords = EKW_554, .skinToneSupport = false},
	EmojiData{ .emoji = "🐵", .name = "monkey face", .group = GRP(2), .keywords = EKW_555, .skinToneSupport = false},
	EmojiData{ .emoji = "🐒", .name = "monkey", .group = GRP(2), .keywords = EKW_556, .skinToneSupport = false},
	EmojiData{ .emoji = "🦍", .name = "gorilla", .group = GRP(2), .keywords = EKW_557, .skinToneSupport = false},
	EmojiData{ .emoji = "🦧", .name = "orangutan", .group = GRP(2), .keywords = EKW_558, .skinToneSupport = false},
	EmojiData{ .emoji = "🐶", .name = "dog face", .group = GRP(2), .keywords = EKW_559, .skinToneSupport = false},
	EmojiData{ .emoji = "🐕", .name = "dog", .group = GRP(2), .keywords = EKW_560, .skinToneSupport = false},
	EmojiData{ .emoji = "🦮", .name = "guide dog", .group = GRP(2), .keywords = EKW_561, .skinToneSupport = false},
	EmojiData{ .emoji = "🐕‍🦺", .name = "service dog", .group = GRP(2), .keywords = EKW_562, .skinToneSupport = false},
	EmojiData{ .emoji = "🐩", .name = "poodle", .group = GRP(2), .keywords = EKW_563, .skinToneSupport = false},
	EmojiData{ .emoji = "🐺", .name = "wolf", .group = GRP(2), .keywords = EKW_564, .skinToneSupport = false},
	EmojiData{ .emoji = "🦊", .name = "fox", .group = GRP(2), .keywords = EKW_565, .skinToneSupport = false},
	EmojiData{ .emoji = "🦝", .name = "raccoon", .group = GRP(2), .keywords = EKW_566, .skinToneSupport = false},
	EmojiData{ .emoji = "🐱", .name = "cat face", .group = GRP(2), .keywords = EKW_567, .skinToneSupport = false},
	EmojiData{ .emoji = "🐈", .name = "cat", .group = GRP(2), .keywords = EKW_568, .skinToneSupport = false},
	EmojiData{ .emoji = "🐈‍⬛", .name = "black cat", .group = GRP(2), .keywords = EKW_569, .skinToneSupport = false},
	EmojiData{ .emoji = "🦁", .name = "lion", .group = GRP(2), .keywords = EKW_570, .skinToneSupport = false},
	EmojiData{ .emoji = "🐯", .name = "tiger face", .group = GRP(2), .keywords = EKW_571, .skinToneSupport = false},
	EmojiData{ .emoji = "🐅", .name = "tiger", .group = GRP(2), .keywords = EKW_572, .skinToneSupport = false},
	EmojiData{ .emoji = "🐆", .name = "leopard", .group = GRP(2), .keywords = EKW_573, .skinToneSupport = false},
	EmojiData{ .emoji = "🐴", .name = "horse face", .group = GRP(2), .keywords = EKW_574, .skinToneSupport = false},
	EmojiData{ .emoji = "🫎", .name = "moose", .group = GRP(2), .keywords = EKW_575, .skinToneSupport = false},
	EmojiData{ .emoji = "🫏", .name = "donkey", .group = GRP(2), .keywords = EKW_576, .skinToneSupport = false},
	EmojiData{ .emoji = "🐎", .name = "horse", .group = GRP(2), .keywords = EKW_577, .skinToneSupport = false},
	EmojiData{ .emoji = "🦄", .name = "unicorn", .group = GRP(2), .keywords = EKW_578, .skinToneSupport = false},
	EmojiData{ .emoji = "🦓", .name = "zebra", .group = GRP(2), .keywords = EKW_579, .skinToneSupport = false},
	EmojiData{ .emoji = "🦌", .name = "deer", .group = GRP(2), .keywords = EKW_580, .skinToneSupport = false},
	EmojiData{ .emoji = "🦬", .name = "bison", .group = GRP(2), .keywords = EKW_581, .skinToneSupport = false},
	EmojiData{ .emoji = "🐮", .name = "cow face", .group = GRP(2), .keywords = EKW_582, .skinToneSupport = false},
	EmojiData{ .emoji = "🐂", .name = "ox", .group = GRP(2), .keywords = EKW_583, .skinToneSupport = false},
	EmojiData{ .emoji = "🐃", .name = "water buffalo", .group = GRP(2), .keywords = EKW_584, .skinToneSupport = false},
	EmojiData{ .emoji = "🐄", .name = "cow", .group = GRP(2), .keywords = EKW_585, .skinToneSupport = false},
	EmojiData{ .emoji = "🐷", .name = "pig face", .group = GRP(2), .keywords = EKW_586, .skinToneSupport = false},
	EmojiData{ .emoji = "🐖", .name = "pig", .group = GRP(2), .keywords = EKW_587, .skinToneSupport = false},
	EmojiData{ .emoji = "🐗", .name = "boar", .group = GRP(2), .keywords = EKW_588, .skinToneSupport = false},
	EmojiData{ .emoji = "🐽", .name = "pig nose", .group = GRP(2), .keywords = EKW_589, .skinToneSupport = false},
	EmojiData{ .emoji = "🐏", .name = "ram", .group = GRP(2), .keywords = EKW_590, .skinToneSupport = false},
	EmojiData{ .emoji = "🐑", .name = "ewe", .group = GRP(2), .keywords = EKW_591, .skinToneSupport = false},
	EmojiData{ .emoji = "🐐", .name = "goat", .group = GRP(2), .keywords = EKW_592, .skinToneSupport = false},
	EmojiData{ .emoji = "🐪", .name = "camel", .group = GRP(2), .keywords = EKW_593, .skinToneSupport = false},
	EmojiData{ .emoji = "🐫", .name = "two-hump camel", .group = GRP(2), .keywords = EKW_594, .skinToneSupport = false},
	EmojiData{ .emoji = "🦙", .name = "llama", .group = GRP(2), .keywords = EKW_595, .skinToneSupport = false},
	EmojiData{ .emoji = "🦒", .name = "giraffe", .group = GRP(2), .keywords = EKW_596, .skinToneSupport = false},
	EmojiData{ .emoji = "🐘", .name = "elephant", .group = GRP(2), .keywords = EKW_597, .skinToneSupport = false},
	EmojiData{ .emoji = "🦣", .name = "mammoth", .group = GRP(2), .keywords = EKW_598, .skinToneSupport = false},
	EmojiData{ .emoji = "🦏", .name = "rhinoceros", .group = GRP(2), .keywords = EKW_599, .skinToneSupport = false},
	EmojiData{ .emoji = "🦛", .name = "hippopotamus", .group = GRP(2), .keywords = EKW_600, .skinToneSupport = false},
	EmojiData{ .emoji = "🐭", .name = "mouse face", .group = GRP(2), .keywords = EKW_601, .skinToneSupport = false},
	EmojiData{ .emoji = "🐁", .name = "mouse", .group = GRP(2), .keywords = EKW_602, .skinToneSupport = false},
	EmojiData{ .emoji = "🐀", .name = "rat", .group = GRP(2), .keywords = EKW_603, .skinToneSupport = false},
	EmojiData{ .emoji = "🐹", .name = "hamster", .group = GRP(2), .keywords = EKW_604, .skinToneSupport = false},
	EmojiData{ .emoji = "🐰", .name = "rabbit face", .group = GRP(2), .keywords = EKW_605, .skinToneSupport = false},
	EmojiData{ .emoji = "🐇", .name = "rabbit", .group = GRP(2), .keywords = EKW_606, .skinToneSupport = false},
	EmojiData{ .emoji = "🐿️", .name = "chipmunk", .group = GRP(2), .keywords = EKW_607, .skinToneSupport = false},
	EmojiData{ .emoji = "🦫", .name = "beaver", .group = GRP(2), .keywords = EKW_608, .skinToneSupport = false},
	EmojiData{ .emoji = "🦔", .name = "hedgehog", .group = GRP(2), .keywords = EKW_609, .skinToneSupport = false},
	EmojiData{ .emoji = "🦇", .name = "bat", .group = GRP(2), .keywords = EKW_610, .skinToneSupport = false},
	EmojiData{ .emoji = "🐻", .name = "bear", .group = GRP(2), .keywords = EKW_611, .skinToneSupport = false},
	EmojiData{ .emoji = "🐻‍❄️", .name = "polar bear", .group = GRP(2), .keywords = EKW_612, .skinToneSupport = false},
	EmojiData{ .emoji = "🐨", .name = "koala", .group = GRP(2), .keywords = EKW_613, .skinToneSupport = false},
	EmojiData{ .emoji = "🐼", .name = "panda", .group = GRP(2), .keywords = EKW_614, .skinToneSupport = false},
	EmojiData{ .emoji = "🦥", .name = "sloth", .group = GRP(2), .keywords = EKW_615, .skinToneSupport = false},
	EmojiData{ .emoji = "🦦", .name = "otter", .group = GRP(2), .keywords = EKW_616, .skinToneSupport = false},
	EmojiData{ .emoji = "🦨", .name = "skunk", .group = GRP(2), .keywords = EKW_617, .skinToneSupport = false},
	EmojiData{ .emoji = "🦘", .name = "kangaroo", .group = GRP(2), .keywords = EKW_618, .skinToneSupport = false},
	EmojiData{ .emoji = "🦡", .name = "badger", .group = GRP(2), .keywords = EKW_619, .skinToneSupport = false},
	EmojiData{ .emoji = "🐾", .name = "paw prints", .group = GRP(2), .keywords = EKW_620, .skinToneSupport = false},
	EmojiData{ .emoji = "🦃", .name = "turkey", .group = GRP(2), .keywords = EKW_621, .skinToneSupport = false},
	EmojiData{ .emoji = "🐔", .name = "chicken", .group = GRP(2), .keywords = EKW_622, .skinToneSupport = false},
	EmojiData{ .emoji = "🐓", .name = "rooster", .group = GRP(2), .keywords = EKW_623, .skinToneSupport = false},
	EmojiData{ .emoji = "🐣", .name = "hatching chick", .group = GRP(2), .keywords = EKW_624, .skinToneSupport = false},
	EmojiData{ .emoji = "🐤", .name = "baby chick", .group = GRP(2), .keywords = EKW_625, .skinToneSupport = false},
	EmojiData{ .emoji = "🐥", .name = "front-facing baby chick", .group = GRP(2), .keywords = EKW_626, .skinToneSupport = false},
	EmojiData{ .emoji = "🐦", .name = "bird", .group = GRP(2), .keywords = EKW_627, .skinToneSupport = false},
	EmojiData{ .emoji = "🐧", .name = "penguin", .group = GRP(2), .keywords = EKW_628, .skinToneSupport = false},
	EmojiData{ .emoji = "🕊️", .name = "dove", .group = GRP(2), .keywords = EKW_629, .skinToneSupport = false},
	EmojiData{ .emoji = "🦅", .name = "eagle", .group = GRP(2), .keywords = EKW_630, .skinToneSupport = false},
	EmojiData{ .emoji = "🦆", .name = "duck", .group = GRP(2), .keywords = EKW_631, .skinToneSupport = false},
	EmojiData{ .emoji = "🦢", .name = "swan", .group = GRP(2), .keywords = EKW_632, .skinToneSupport = false},
	EmojiData{ .emoji = "🦉", .name = "owl", .group = GRP(2), .keywords = EKW_633, .skinToneSupport = false},
	EmojiData{ .emoji = "🦤", .name = "dodo", .group = GRP(2), .keywords = EKW_634, .skinToneSupport = false},
	EmojiData{ .emoji = "🪶", .name = "feather", .group = GRP(2), .keywords = EKW_635, .skinToneSupport = false},
	EmojiData{ .emoji = "🦩", .name = "flamingo", .group = GRP(2), .keywords = EKW_636, .skinToneSupport = false},
	EmojiData{ .emoji = "🦚", .name = "peacock", .group = GRP(2), .keywords = EKW_637, .skinToneSupport = false},
	EmojiData{ .emoji = "🦜", .name = "parrot", .group = GRP(2), .keywords = EKW_638, .skinToneSupport = false},
	EmojiData{ .emoji = "🪽", .name = "wing", .group = GRP(2), .keywords = EKW_639, .skinToneSupport = false},
	EmojiData{ .emoji = "🐦‍⬛", .name = "black bird", .group = GRP(2), .keywords = EKW_640, .skinToneSupport = false},
	EmojiData{ .emoji = "🪿", .name = "goose", .group = GRP(2), .keywords = EKW_641, .skinToneSupport = false},
	EmojiData{ .emoji = "🐦‍🔥", .name = "phoenix", .group = GRP(2), .keywords = EKW_642, .skinToneSupport = false},
	EmojiData{ .emoji = "🐸", .name = "frog", .group = GRP(2), .keywords = EKW_643, .skinToneSupport = false},
	EmojiData{ .emoji = "🐊", .name = "crocodile", .group = GRP(2), .keywords = EKW_644, .skinToneSupport = false},
	EmojiData{ .emoji = "🐢", .name = "turtle", .group = GRP(2), .keywords = EKW_645, .skinToneSupport = false},
	EmojiData{ .emoji = "🦎", .name = "lizard", .group = GRP(2), .keywords = EKW_646, .skinToneSupport = false},
	EmojiData{ .emoji = "🐍", .name = "snake", .group = GRP(2), .keywords = EKW_647, .skinToneSupport = false},
	EmojiData{ .emoji = "🐲", .name = "dragon face", .group = GRP(2), .keywords = EKW_648, .skinToneSupport = false},
	EmojiData{ .emoji = "🐉", .name = "dragon", .group = GRP(2), .keywords = EKW_649, .skinToneSupport = false},
	EmojiData{ .emoji = "🦕", .name = "sauropod", .group = GRP(2), .keywords = EKW_650, .skinToneSupport = false},
	EmojiData{ .emoji = "🦖", .name = "T-Rex", .group = GRP(2), .keywords = EKW_651, .skinToneSupport = false},
	EmojiData{ .emoji = "🐳", .name = "spouting whale", .group = GRP(2), .keywords = EKW_652, .skinToneSupport = false},
	EmojiData{ .emoji = "🐋", .name = "whale", .group = GRP(2), .keywords = EKW_653, .skinToneSupport = false},
	EmojiData{ .emoji = "🐬", .name = "dolphin", .group = GRP(2), .keywords = EKW_654, .skinToneSupport = false},
	EmojiData{ .emoji = "🦭", .name = "seal", .group = GRP(2), .keywords = EKW_655, .skinToneSupport = false},
	EmojiData{ .emoji = "🐟", .name = "fish", .group = GRP(2), .keywords = EKW_656, .skinToneSupport = false},
	EmojiData{ .emoji = "🐠", .name = "tropical fish", .group = GRP(2), .keywords = EKW_657, .skinToneSupport = false},
	EmojiData{ .emoji = "🐡", .name = "blowfish", .group = GRP(2), .keywords = EKW_658, .skinToneSupport = false},
	EmojiData{ .emoji = "🦈", .name = "shark", .group = GRP(2), .keywords = EKW_659, .skinToneSupport = false},
	EmojiData{ .emoji = "🐙", .name = "octopus", .group = GRP(2), .keywords = EKW_660, .skinToneSupport = false},
	EmojiData{ .emoji = "🐚", .name = "spiral shell", .group = GRP(2), .keywords = EKW_661, .skinToneSupport = false},
	EmojiData{ .emoji = "🪸", .name = "coral", .group = GRP(2), .keywords = EKW_662, .skinToneSupport = false},
	EmojiData{ .emoji = "🪼", .name = "jellyfish", .group = GRP(2), .keywords = EKW_663, .skinToneSupport = false},
	EmojiData{ .emoji = "🦀", .name = "crab", .group = GRP(2), .keywords = EKW_664, .skinToneSupport = false},
	EmojiData{ .emoji = "🦞", .name = "lobster", .group = GRP(2), .keywords = EKW_665, .skinToneSupport = false},
	EmojiData{ .emoji = "🦐", .name = "shrimp", .group = GRP(2), .keywords = EKW_666, .skinToneSupport = false},
	EmojiData{ .emoji = "🦑", .name = "squid", .group = GRP(2), .keywords = EKW_667, .skinToneSupport = false},
	EmojiData{ .emoji = "🦪", .name = "oyster", .group = GRP(2), .keywords = EKW_668, .skinToneSupport = false},
	EmojiData{ .emoji = "🐌", .name = "snail", .group = GRP(2), .keywords = EKW_669, .skinToneSupport = false},
	EmojiData{ .emoji = "🦋", .name = "butterfly", .group = GRP(2), .keywords = EKW_670, .skinToneSupport = false},
	EmojiData{ .emoji = "🐛", .name = "bug", .group = GRP(2), .keywords = EKW_671, .skinToneSupport = false},
	EmojiData{ .emoji = "🐜", .name = "ant", .group = GRP(2), .keywords = EKW_672, .skinToneSupport = false},
	EmojiData{ .emoji = "🐝", .name = "honeybee", .group = GRP(2), .keywords = EKW_673, .skinToneSupport = false},
	EmojiData{ .emoji = "🪲", .name = "beetle", .group = GRP(2), .keywords = EKW_674, .skinToneSupport = false},
	EmojiData{ .emoji = "🐞", .name = "lady beetle", .group = GRP(2), .keywords = EKW_675, .skinToneSupport = false},
	EmojiData{ .emoji = "🦗", .name = "cricket", .group = GRP(2), .keywords = EKW_676, .skinToneSupport = false},
	EmojiData{ .emoji = "🪳", .name = "cockroach", .group = GRP(2), .keywords = EKW_677, .skinToneSupport = false},
	EmojiData{ .emoji = "🕷️", .name = "spider", .group = GRP(2), .keywords = EKW_678, .skinToneSupport = false},
	EmojiData{ .emoji = "🕸️", .name = "spider web", .group = GRP(2), .keywords = EKW_679, .skinToneSupport = false},
	EmojiData{ .emoji = "🦂", .name = "scorpion", .group = GRP(2), .keywords = EKW_680, .skinToneSupport = false},
	EmojiData{ .emoji = "🦟", .name = "mosquito", .group = GRP(2), .keywords = EKW_681, .skinToneSupport = false},
	EmojiData{ .emoji = "🪰", .name = "fly", .group = GRP(2), .keywords = EKW_682, .skinToneSupport = false},
	EmojiData{ .emoji = "🪱", .name = "worm", .group = GRP(2), .keywords = EKW_683, .skinToneSupport = false},
	EmojiData{ .emoji = "🦠", .name = "microbe", .group = GRP(2), .keywords = EKW_684, .skinToneSupport = false},
	EmojiData{ .emoji = "💐", .name = "bouquet", .group = GRP(2), .keywords = EKW_685, .skinToneSupport = false},
	EmojiData{ .emoji = "🌸", .name = "cherry blossom", .group = GRP(2), .keywords = EKW_686, .skinToneSupport = false},
	EmojiData{ .emoji = "💮", .name = "white flower", .group = GRP(2), .keywords = EKW_687, .skinToneSupport = false},
	EmojiData{ .emoji = "🪷", .name = "lotus", .group = GRP(2), .keywords = EKW_688, .skinToneSupport = false},
	EmojiData{ .emoji = "🏵️", .name = "rosette", .group = GRP(2), .keywords = EKW_689, .skinToneSupport = false},
	EmojiData{ .emoji = "🌹", .name = "rose", .group = GRP(2), .keywords = EKW_690, .skinToneSupport = false},
	EmojiData{ .emoji = "🥀", .name = "wilted flower", .group = GRP(2), .keywords = EKW_691, .skinToneSupport = false},
	EmojiData{ .emoji = "🌺", .name = "hibiscus", .group = GRP(2), .keywords = EKW_692, .skinToneSupport = false},
	EmojiData{ .emoji = "🌻", .name = "sunflower", .group = GRP(2), .keywords = EKW_693, .skinToneSupport = false},
	EmojiData{ .emoji = "🌼", .name = "blossom", .group = GRP(2), .keywords = EKW_694, .skinToneSupport = false},
	EmojiData{ .emoji = "🌷", .name = "tulip", .group = GRP(2), .keywords = EKW_695, .skinToneSupport = false},
	EmojiData{ .emoji = "🪻", .name = "hyacinth", .group = GRP(2), .keywords = EKW_696, .skinToneSupport = false},
	EmojiData{ .emoji = "🌱", .name = "seedling", .group = GRP(2), .keywords = EKW_697, .skinToneSupport = false},
	EmojiData{ .emoji = "🪴", .name = "potted plant", .group = GRP(2), .keywords = EKW_698, .skinToneSupport = false},
	EmojiData{ .emoji = "🌲", .name = "evergreen tree", .group = GRP(2), .keywords = EKW_699, .skinToneSupport = false},
	EmojiData{ .emoji = "🌳", .name = "deciduous tree", .group = GRP(2), .keywords = EKW_700, .skinToneSupport = false},
	EmojiData{ .emoji = "🌴", .name = "palm tree", .group = GRP(2), .keywords = EKW_701, .skinToneSupport = false},
	EmojiData{ .emoji = "🌵", .name = "cactus", .group = GRP(2), .keywords = EKW_702, .skinToneSupport = false},
	EmojiData{ .emoji = "🌾", .name = "sheaf of rice", .group = GRP(2), .keywords = EKW_703, .skinToneSupport = false},
	EmojiData{ .emoji = "🌿", .name = "herb", .group = GRP(2), .keywords = EKW_704, .skinToneSupport = false},
	EmojiData{ .emoji = "☘️", .name = "shamrock", .group = GRP(2), .keywords = EKW_705, .skinToneSupport = false},
	EmojiData{ .emoji = "🍀", .name = "four leaf clover", .group = GRP(2), .keywords = EKW_706, .skinToneSupport = false},
	EmojiData{ .emoji = "🍁", .name = "maple leaf", .group = GRP(2), .keywords = EKW_707, .skinToneSupport = false},
	EmojiData{ .emoji = "🍂", .name = "fallen leaf", .group = GRP(2), .keywords = EKW_708, .skinToneSupport = false},
	EmojiData{ .emoji = "🍃", .name = "leaf fluttering in wind", .group = GRP(2), .keywords = EKW_709, .skinToneSupport = false},
	EmojiData{ .emoji = "🪹", .name = "empty nest", .group = GRP(2), .keywords = EKW_710, .skinToneSupport = false},
	EmojiData{ .emoji = "🪺", .name = "nest with eggs", .group = GRP(2), .keywords = EKW_711, .skinToneSupport = false},
	EmojiData{ .emoji = "🍄", .name = "mushroom", .group = GRP(2), .keywords = EKW_712, .skinToneSupport = false},
	EmojiData{ .emoji = "🪾", .name = "leafless tree", .group = GRP(2), .keywords = EKW_713, .skinToneSupport = false},
	EmojiData{ .emoji = "🍇", .name = "grapes", .group = GRP(3), .keywords = EKW_714, .skinToneSupport = false},
	EmojiData{ .emoji = "🍈", .name = "melon", .group = GRP(3), .keywords = EKW_715, .skinToneSupport = false},
	EmojiData{ .emoji = "🍉", .name = "watermelon", .group = GRP(3), .keywords = EKW_716, .skinToneSupport = false},
	EmojiData{ .emoji = "🍊", .name = "tangerine", .group = GRP(3), .keywords = EKW_717, .skinToneSupport = false},
	EmojiData{ .emoji = "🍋", .name = "lemon", .group = GRP(3), .keywords = EKW_718, .skinToneSupport = false},
	EmojiData{ .emoji = "🍋‍🟩", .name = "lime", .group = GRP(3), .keywords = EKW_719, .skinToneSupport = false},
	EmojiData{ .emoji = "🍌", .name = "banana", .group = GRP(3), .keywords = EKW_720, .skinToneSupport = false},
	EmojiData{ .emoji = "🍍", .name = "pineapple", .group = GRP(3), .keywords = EKW_721, .skinToneSupport = false},
	EmojiData{ .emoji = "🥭", .name = "mango", .group = GRP(3), .keywords = EKW_722, .skinToneSupport = false},
	EmojiData{ .emoji = "🍎", .name = "red apple", .group = GRP(3), .keywords = EKW_723, .skinToneSupport = false},
	EmojiData{ .emoji = "🍏", .name = "green apple", .group = GRP(3), .keywords = EKW_724, .skinToneSupport = false},
	EmojiData{ .emoji = "🍐", .name = "pear", .group = GRP(3), .keywords = EKW_725, .skinToneSupport = false},
	EmojiData{ .emoji = "🍑", .name = "peach", .group = GRP(3), .keywords = EKW_726, .skinToneSupport = false},
	EmojiData{ .emoji = "🍒", .name = "cherries", .group = GRP(3), .keywords = EKW_727, .skinToneSupport = false},
	EmojiData{ .emoji = "🍓", .name = "strawberry", .group = GRP(3), .keywords = EKW_728, .skinToneSupport = false},
	EmojiData{ .emoji = "🫐", .name = "blueberries", .group = GRP(3), .keywords = EKW_729, .skinToneSupport = false},
	EmojiData{ .emoji = "🥝", .name = "kiwi fruit", .group = GRP(3), .keywords = EKW_730, .skinToneSupport = false},
	EmojiData{ .emoji = "🍅", .name = "tomato", .group = GRP(3), .keywords = EKW_731, .skinToneSupport = false},
	EmojiData{ .emoji = "🫒", .name = "olive", .group = GRP(3), .keywords = EKW_732, .skinToneSupport = false},
	EmojiData{ .emoji = "🥥", .name = "coconut", .group = GRP(3), .keywords = EKW_733, .skinToneSupport = false},
	EmojiData{ .emoji = "🥑", .name = "avocado", .group = GRP(3), .keywords = EKW_734, .skinToneSupport = false},
	EmojiData{ .emoji = "🍆", .name = "eggplant", .group = GRP(3), .keywords = EKW_735, .skinToneSupport = false},
	EmojiData{ .emoji = "🥔", .name = "potato", .group = GRP(3), .keywords = EKW_736, .skinToneSupport = false},
	EmojiData{ .emoji = "🥕", .name = "carrot", .group = GRP(3), .keywords = EKW_737, .skinToneSupport = false},
	EmojiData{ .emoji = "🌽", .name = "ear of corn", .group = GRP(3), .keywords = EKW_738, .skinToneSupport = false},
	EmojiData{ .emoji = "🌶️", .name = "hot pepper", .group = GRP(3), .keywords = EKW_739, .skinToneSupport = false},
	EmojiData{ .emoji = "🫑", .name = "bell pepper", .group = GRP(3), .keywords = EKW_740, .skinToneSupport = false},
	EmojiData{ .emoji = "🥒", .name = "cucumber", .group = GRP(3), .keywords = EKW_741, .skinToneSupport = false},
	EmojiData{ .emoji = "🥬", .name = "leafy green", .group = GRP(3), .keywords = EKW_742, .skinToneSupport = false},
	EmojiData{ .emoji = "🥦", .name = "broccoli", .group = GRP(3), .keywords = EKW_743, .skinToneSupport = false},
	EmojiData{ .emoji = "🧄", .name = "garlic", .group = GRP(3), .keywords = EKW_744, .skinToneSupport = false},
	EmojiData{ .emoji = "🧅", .name = "onion", .group = GRP(3), .keywords = EKW_745, .skinToneSupport = false},
	EmojiData{ .emoji = "🥜", .name = "peanuts", .group = GRP(3), .keywords = EKW_746, .skinToneSupport = false},
	EmojiData{ .emoji = "🫘", .name = "beans", .group = GRP(3), .keywords = EKW_747, .skinToneSupport = false},
	EmojiData{ .emoji = "🌰", .name = "chestnut", .group = GRP(3), .keywords = EKW_748, .skinToneSupport = false},
	EmojiData{ .emoji = "🫚", .name = "ginger root", .group = GRP(3), .keywords = EKW_749, .skinToneSupport = false},
	EmojiData{ .emoji = "🫛", .name = "pea pod", .group = GRP(3), .keywords = EKW_750, .skinToneSupport = false},
	EmojiData{ .emoji = "🍄‍🟫", .name = "brown mushroom", .group = GRP(3), .keywords = EKW_751, .skinToneSupport = false},
	EmojiData{ .emoji = "🫜", .name = "root vegetable", .group = GRP(3), .keywords = EKW_752, .skinToneSupport = false},
	EmojiData{ .emoji = "🍞", .name = "bread", .group = GRP(3), .keywords = EKW_753, .skinToneSupport = false},
	EmojiData{ .emoji = "🥐", .name = "croissant", .group = GRP(3), .keywords = EKW_754, .skinToneSupport = false},
	EmojiData{ .emoji = "🥖", .name = "baguette bread", .group = GRP(3), .keywords = EKW_755, .skinToneSupport = false},
	EmojiData{ .emoji = "🫓", .name = "flatbread", .group = GRP(3), .keywords = EKW_756, .skinToneSupport = false},
	EmojiData{ .emoji = "🥨", .name = "pretzel", .group = GRP(3), .keywords = EKW_757, .skinToneSupport = false},
	EmojiData{ .emoji = "🥯", .name = "bagel", .group = GRP(3), .keywords = EKW_758, .skinToneSupport = false},
	EmojiData{ .emoji = "🥞", .name = "pancakes", .group = GRP(3), .keywords = EKW_759, .skinToneSupport = false},
	EmojiData{ .emoji = "🧇", .name = "waffle", .group = GRP(3), .keywords = EKW_760, .skinToneSupport = false},
	EmojiData{ .emoji = "🧀", .name = "cheese wedge", .group = GRP(3), .keywords = EKW_761, .skinToneSupport = false},
	EmojiData{ .emoji = "🍖", .name = "meat on bone", .group = GRP(3), .keywords = EKW_762, .skinToneSupport = false},
	EmojiData{ .emoji = "🍗", .name = "poultry leg", .group = GRP(3), .keywords = EKW_763, .skinToneSupport = false},
	EmojiData{ .emoji = "🥩", .name = "cut of meat", .group = GRP(3), .keywords = EKW_764, .skinToneSupport = false},
	EmojiData{ .emoji = "🥓", .name = "bacon", .group = GRP(3), .keywords = EKW_765, .skinToneSupport = false},
	EmojiData{ .emoji = "🍔", .name = "hamburger", .group = GRP(3), .keywords = EKW_766, .skinToneSupport = false},
	EmojiData{ .emoji = "🍟", .name = "french fries", .group = GRP(3), .keywords = EKW_767, .skinToneSupport = false},
	EmojiData{ .emoji = "🍕", .name = "pizza", .group = GRP(3), .keywords = EKW_768, .skinToneSupport = false},
	EmojiData{ .emoji = "🌭", .name = "hot dog", .group = GRP(3), .keywords = EKW_769, .skinToneSupport = false},
	EmojiData{ .emoji = "🥪", .name = "sandwich", .group = GRP(3), .keywords = EKW_770, .skinToneSupport = false},
	EmojiData{ .emoji = "🌮", .name = "taco", .group = GRP(3), .keywords = EKW_771, .skinToneSupport = false},
	EmojiData{ .emoji = "🌯", .name = "burrito", .group = GRP(3), .keywords = EKW_772, .skinToneSupport = false},
	EmojiData{ .emoji = "🫔", .name = "tamale", .group = GRP(3), .keywords = EKW_773, .skinToneSupport = false},
	EmojiData{ .emoji = "🥙", .name = "stuffed flatbread", .group = GRP(3), .keywords = EKW_774, .skinToneSupport = false},
	EmojiData{ .emoji = "🧆", .name = "falafel", .group = GRP(3), .keywords = EKW_775, .skinToneSupport = false},
	EmojiData{ .emoji = "🥚", .name = "egg", .group = GRP(3), .keywords = EKW_776, .skinToneSupport = false},
	EmojiData{ .emoji = "🍳", .name = "cooking", .group = GRP(3), .keywords = EKW_777, .skinToneSupport = false},
	EmojiData{ .emoji = "🥘", .name = "shallow pan of food", .group = GRP(3), .keywords = EKW_778, .skinToneSupport = false},
	EmojiData{ .emoji = "🍲", .name = "pot of food", .group = GRP(3), .keywords = EKW_779, .skinToneSupport = false},
	EmojiData{ .emoji = "🫕", .name = "fondue", .group = GRP(3), .keywords = EKW_780, .skinToneSupport = false},
	EmojiData{ .emoji = "🥣", .name = "bowl with spoon", .group = GRP(3), .keywords = EKW_781, .skinToneSupport = false},
	EmojiData{ .emoji = "🥗", .name = "green salad", .group = GRP(3), .keywords = EKW_782, .skinToneSupport = false},
	EmojiData{ .emoji = "🍿", .name = "popcorn", .group = GRP(3), .keywords = EKW_783, .skinToneSupport = false},
	EmojiData{ .emoji = "🧈", .name = "butter", .group = GRP(3), .keywords = EKW_784, .skinToneSupport = false},
	EmojiData{ .emoji = "🧂", .name = "salt", .group = GRP(3), .keywords = EKW_785, .skinToneSupport = false},
	EmojiData{ .emoji = "🥫", .name = "canned food", .group = GRP(3), .keywords = EKW_786, .skinToneSupport = false},
	EmojiData{ .emoji = "🍱", .name = "bento box", .group = GRP(3), .keywords = EKW_787, .skinToneSupport = false},
	EmojiData{ .emoji = "🍘", .name = "rice cracker", .group = GRP(3), .keywords = EKW_788, .skinToneSupport = false},
	EmojiData{ .emoji = "🍙", .name = "rice ball", .group = GRP(3), .keywords = EKW_789, .skinToneSupport = false},
	EmojiData{ .emoji = "🍚", .name = "cooked rice", .group = GRP(3), .keywords = EKW_790, .skinToneSupport = false},
	EmojiData{ .emoji = "🍛", .name = "curry rice", .group = GRP(3), .keywords = EKW_791, .skinToneSupport = false},
	EmojiData{ .emoji = "🍜", .name = "steaming bowl", .group = GRP(3), .keywords = EKW_792, .skinToneSupport = false},
	EmojiData{ .emoji = "🍝", .name = "spaghetti", .group = GRP(3), .keywords = EKW_793, .skinToneSupport = false},
	EmojiData{ .emoji = "🍠", .name = "roasted sweet potato", .group = GRP(3), .keywords = EKW_794, .skinToneSupport = false},
	EmojiData{ .emoji = "🍢", .name = "oden", .group = GRP(3), .keywords = EKW_795, .skinToneSupport = false},
	EmojiData{ .emoji = "🍣", .name = "sushi", .group = GRP(3), .keywords = EKW_796, .skinToneSupport = false},
	EmojiData{ .emoji = "🍤", .name = "fried shrimp", .group = GRP(3), .keywords = EKW_797, .skinToneSupport = false},
	EmojiData{ .emoji = "🍥", .name = "fish cake with swirl", .group = GRP(3), .keywords = EKW_798, .skinToneSupport = false},
	EmojiData{ .emoji = "🥮", .name = "moon cake", .group = GRP(3), .keywords = EKW_799, .skinToneSupport = false},
	EmojiData{ .emoji = "🍡", .name = "dango", .group = GRP(3), .keywords = EKW_800, .skinToneSupport = false},
	EmojiData{ .emoji = "🥟", .name = "dumpling", .group = GRP(3), .keywords = EKW_801, .skinToneSupport = false},
	EmojiData{ .emoji = "🥠", .name = "fortune cookie", .group = GRP(3), .keywords = EKW_802, .skinToneSupport = false},
	EmojiData{ .emoji = "🥡", .name = "takeout box", .group = GRP(3), .keywords = EKW_803, .skinToneSupport = false},
	EmojiData{ .emoji = "🍦", .name = "soft ice cream", .group = GRP(3), .keywords = EKW_804, .skinToneSupport = false},
	EmojiData{ .emoji = "🍧", .name = "shaved ice", .group = GRP(3), .keywords = EKW_805, .skinToneSupport = false},
	EmojiData{ .emoji = "🍨", .name = "ice cream", .group = GRP(3), .keywords = EKW_806, .skinToneSupport = false},
	EmojiData{ .emoji = "🍩", .name = "doughnut", .group = GRP(3), .keywords = EKW_807, .skinToneSupport = false},
	EmojiData{ .emoji = "🍪", .name = "cookie", .group = GRP(3), .keywords = EKW_808, .skinToneSupport = false},
	EmojiData{ .emoji = "🎂", .name = "birthday cake", .group = GRP(3), .keywords = EKW_809, .skinToneSupport = false},
	EmojiData{ .emoji = "🍰", .name = "shortcake", .group = GRP(3), .keywords = EKW_810, .skinToneSupport = false},
	EmojiData{ .emoji = "🧁", .name = "cupcake", .group = GRP(3), .keywords = EKW_811, .skinToneSupport = false},
	EmojiData{ .emoji = "🥧", .name = "pie", .group = GRP(3), .keywords = EKW_812, .skinToneSupport = false},
	EmojiData{ .emoji = "🍫", .name = "chocolate bar", .group = GRP(3), .keywords = EKW_813, .skinToneSupport = false},
	EmojiData{ .emoji = "🍬", .name = "candy", .group = GRP(3), .keywords = EKW_814, .skinToneSupport = false},
	EmojiData{ .emoji = "🍭", .name = "lollipop", .group = GRP(3), .keywords = EKW_815, .skinToneSupport = false},
	EmojiData{ .emoji = "🍮", .name = "custard", .group = GRP(3), .keywords = EKW_816, .skinToneSupport = false},
	EmojiData{ .emoji = "🍯", .name = "honey pot", .group = GRP(3), .keywords = EKW_817, .skinToneSupport = false},
	EmojiData{ .emoji = "🍼", .name = "baby bottle", .group = GRP(3), .keywords = EKW_818, .skinToneSupport = false},
	EmojiData{ .emoji = "🥛", .name = "glass of milk", .group = GRP(3), .keywords = EKW_819, .skinToneSupport = false},
	EmojiData{ .emoji = "☕", .name = "hot beverage", .group = GRP(3), .keywords = EKW_820, .skinToneSupport = false},
	EmojiData{ .emoji = "🫖", .name = "teapot", .group = GRP(3), .keywords = EKW_821, .skinToneSupport = false},
	EmojiData{ .emoji = "🍵", .name = "teacup without handle", .group = GRP(3), .keywords = EKW_822, .skinToneSupport = false},
	EmojiData{ .emoji = "🍶", .name = "sake", .group = GRP(3), .keywords = EKW_823, .skinToneSupport = false},
	EmojiData{ .emoji = "🍾", .name = "bottle with popping cork", .group = GRP(3), .keywords = EKW_824, .skinToneSupport = false},
	EmojiData{ .emoji = "🍷", .name = "wine glass", .group = GRP(3), .keywords = EKW_825, .skinToneSupport = false},
	EmojiData{ .emoji = "🍸", .name = "cocktail glass", .group = GRP(3), .keywords = EKW_826, .skinToneSupport = false},
	EmojiData{ .emoji = "🍹", .name = "tropical drink", .group = GRP(3), .keywords = EKW_827, .skinToneSupport = false},
	EmojiData{ .emoji = "🍺", .name = "beer mug", .group = GRP(3), .keywords = EKW_828, .skinToneSupport = false},
	EmojiData{ .emoji = "🍻", .name = "clinking beer mugs", .group = GRP(3), .keywords = EKW_829, .skinToneSupport = false},
	EmojiData{ .emoji = "🥂", .name = "clinking glasses", .group = GRP(3), .keywords = EKW_830, .skinToneSupport = false},
	EmojiData{ .emoji = "🥃", .name = "tumbler glass", .group = GRP(3), .keywords = EKW_831, .skinToneSupport = false},
	EmojiData{ .emoji = "🫗", .name = "pouring liquid", .group = GRP(3), .keywords = EKW_832, .skinToneSupport = false},
	EmojiData{ .emoji = "🥤", .name = "cup with straw", .group = GRP(3), .keywords = EKW_833, .skinToneSupport = false},
	EmojiData{ .emoji = "🧋", .name = "bubble tea", .group = GRP(3), .keywords = EKW_834, .skinToneSupport = false},
	EmojiData{ .emoji = "🧃", .name = "beverage box", .group = GRP(3), .keywords = EKW_835, .skinToneSupport = false},
	EmojiData{ .emoji = "🧉", .name = "mate", .group = GRP(3), .keywords = EKW_836, .skinToneSupport = false},
	EmojiData{ .emoji = "🧊", .name = "ice", .group = GRP(3), .keywords = EKW_837, .skinToneSupport = false},
	EmojiData{ .emoji = "🥢", .name = "chopsticks", .group = GRP(3), .keywords = EKW_838, .skinToneSupport = false},
	EmojiData{ .emoji = "🍽️", .name = "fork and knife with plate", .group = GRP(3), .keywords = EKW_839, .skinToneSupport = false},
	EmojiData{ .emoji = "🍴", .name = "fork and knife", .group = GRP(3), .keywords = EKW_840, .skinToneSupport = false},
	EmojiData{ .emoji = "🥄", .name = "spoon", .group = GRP(3), .keywords = EKW_841, .skinToneSupport = false},
	EmojiData{ .emoji = "🔪", .name = "kitchen knife", .group = GRP(3), .keywords = EKW_842, .skinToneSupport = false},
	EmojiData{ .emoji = "🫙", .name = "jar", .group = GRP(3), .keywords = EKW_843, .skinToneSupport = false},
	EmojiData{ .emoji = "🏺", .name = "amphora", .group = GRP(3), .keywords = EKW_844, .skinToneSupport = false},
	EmojiData{ .emoji = "🌍", .name = "globe showing Europe-Africa", .group = GRP(4), .keywords = EKW_845, .skinToneSupport = false},
	EmojiData{ .emoji = "🌎", .name = "globe showing Americas", .group = GRP(4), .keywords = EKW_846, .skinToneSupport = false},
	EmojiData{ .emoji = "🌏", .name = "globe showing Asia-Australia", .group = GRP(4), .keywords = EKW_847, .skinToneSupport = false},
	EmojiData{ .emoji = "🌐", .name = "globe with meridians", .group = GRP(4), .keywords = EKW_848, .skinToneSupport = false},
	EmojiData{ .emoji = "🗺️", .name = "world map", .group = GRP(4), .keywords = EKW_849, .skinToneSupport = false},
	EmojiData{ .emoji = "🗾", .name = "map of Japan", .group = GRP(4), .keywords = EKW_850, .skinToneSupport = false},
	EmojiData{ .emoji = "🧭", .name = "compass", .group = GRP(4), .keywords = EKW_851, .skinToneSupport = false},
	EmojiData{ .emoji = "🏔️", .name = "snow-capped mountain", .group = GRP(4), .keywords = EKW_852, .skinToneSupport = false},
	EmojiData{ .emoji = "⛰️", .name = "mountain", .group = GRP(4), .keywords = EKW_853, .skinToneSupport = false},
	EmojiData{ .emoji = "🌋", .name = "volcano", .group = GRP(4), .keywords = EKW_854, .skinToneSupport = false},
	EmojiData{ .emoji = "🗻", .name = "mount fuji", .group = GRP(4), .keywords = EKW_855, .skinToneSupport = false},
	EmojiData{ .emoji = "🏕️", .name = "camping", .group = GRP(4), .keywords = EKW_856, .skinToneSupport = false},
	EmojiData{ .emoji = "🏖️", .name = "beach with umbrella", .group = GRP(4), .keywords = EKW_857, .skinToneSupport = false},
	EmojiData{ .emoji = "🏜️", .name = "desert", .group = GRP(4), .keywords = EKW_858, .skinToneSupport = false},
	EmojiData{ .emoji = "🏝️", .name = "desert island", .group = GRP(4), .keywords = EKW_859, .skinToneSupport = false},
	EmojiData{ .emoji = "🏞️", .name = "national park", .group = GRP(4), .keywords = EKW_860, .skinToneSupport = false},
	EmojiData{ .emoji = "🏟️", .name = "stadium", .group = GRP(4), .keywords = EKW_861, .skinToneSupport = false},
	EmojiData{ .emoji = "🏛️", .name = "classical building", .group = GRP(4), .keywords = EKW_862, .skinToneSupport = false},
	EmojiData{ .emoji = "🏗️", .name = "building construction", .group = GRP(4), .keywords = EKW_863, .skinToneSupport = false},
	EmojiData{ .emoji = "🧱", .name = "brick", .group = GRP(4), .keywords = EKW_864, .skinToneSupport = false},
	EmojiData{ .emoji = "🪨", .name = "rock", .group = GRP(4), .keywords = EKW_865, .skinToneSupport = false},
	EmojiData{ .emoji = "🪵", .name = "wood", .group = GRP(4), .keywords = EKW_866, .skinToneSupport = false},
	EmojiData{ .emoji = "🛖", .name = "hut", .group = GRP(4), .keywords = EKW_867, .skinToneSupport = false},
	EmojiData{ .emoji = "🏘️", .name = "houses", .group = GRP(4), .keywords = EKW_868, .skinToneSupport = false},
	EmojiData{ .emoji = "🏚️", .name = "derelict house", .group = GRP(4), .keywords = EKW_869, .skinToneSupport = false},
	EmojiData{ .emoji = "🏠", .name = "house", .group = GRP(4), .keywords = EKW_870, .skinToneSupport = false},
	EmojiData{ .emoji = "🏡", .name = "house with garden", .group = GRP(4), .keywords = EKW_871, .skinToneSupport = false},
	EmojiData{ .emoji = "🏢", .name = "office building", .group = GRP(4), .keywords = EKW_872, .skinToneSupport = false},
	EmojiData{ .emoji = "🏣", .name = "Japanese post office", .group = GRP(4), .keywords = EKW_873, .skinToneSupport = false},
	EmojiData{ .emoji = "🏤", .name = "post office", .group = GRP(4), .keywords = EKW_874, .skinToneSupport = false},
	EmojiData{ .emoji = "🏥", .name = "hospital", .group = GRP(4), .keywords = EKW_875, .skinToneSupport = false},
	EmojiData{ .emoji = "🏦", .name = "bank", .group = GRP(4), .keywords = EKW_876, .skinToneSupport = false},
	EmojiData{ .emoji = "🏨", .name = "hotel", .group = GRP(4), .keywords = EKW_877, .skinToneSupport = false},
	EmojiData{ .emoji = "🏩", .name = "love hotel", .group = GRP(4), .keywords = EKW_878, .skinToneSupport = false},
	EmojiData{ .emoji = "🏪", .name = "convenience store", .group = GRP(4), .keywords = EKW_879, .skinToneSupport = false},
	EmojiData{ .emoji = "🏫", .name = "school", .group = GRP(4), .keywords = EKW_880, .skinToneSupport = false},
	EmojiData{ .emoji = "🏬", .name = "department store", .group = GRP(4), .keywords = EKW_881, .skinToneSupport = false},
	EmojiData{ .emoji = "🏭", .name = "factory", .group = GRP(4), .keywords = EKW_882, .skinToneSupport = false},
	EmojiData{ .emoji = "🏯", .name = "Japanese castle", .group = GRP(4), .keywords = EKW_883, .skinToneSupport = false},
	EmojiData{ .emoji = "🏰", .name = "castle", .group = GRP(4), .keywords = EKW_884, .skinToneSupport = false},
	EmojiData{ .emoji = "💒", .name = "wedding", .group = GRP(4), .keywords = EKW_885, .skinToneSupport = false},
	EmojiData{ .emoji = "🗼", .name = "Tokyo tower", .group = GRP(4), .keywords = EKW_886, .skinToneSupport = false},
	EmojiData{ .emoji = "🗽", .name = "Statue of Liberty", .group = GRP(4), .keywords = EKW_887, .skinToneSupport = false},
	EmojiData{ .emoji = "⛪", .name = "church", .group = GRP(4), .keywords = EKW_888, .skinToneSupport = false},
	EmojiData{ .emoji = "🕌", .name = "mosque", .group = GRP(4), .keywords = EKW_889, .skinToneSupport = false},
	EmojiData{ .emoji = "🛕", .name = "hindu temple", .group = GRP(4), .keywords = EKW_890, .skinToneSupport = false},
	EmojiData{ .emoji = "🕍", .name = "synagogue", .group = GRP(4), .keywords = EKW_891, .skinToneSupport = false},
	EmojiData{ .emoji = "⛩️", .name = "shinto shrine", .group = GRP(4), .keywords = EKW_892, .skinToneSupport = false},
	EmojiData{ .emoji = "🕋", .name = "kaaba", .group = GRP(4), .keywords = EKW_893, .skinToneSupport = false},
	EmojiData{ .emoji = "⛲", .name = "fountain", .group = GRP(4), .keywords = EKW_894, .skinToneSupport = false},
	EmojiData{ .emoji = "⛺", .name = "tent", .group = GRP(4), .keywords = EKW_895, .skinToneSupport = false},
	EmojiData{ .emoji = "🌁", .name = "foggy", .group = GRP(4), .keywords = EKW_896, .skinToneSupport = false},
	EmojiData{ .emoji = "🌃", .name = "night with stars", .group = GRP(4), .keywords = EKW_897, .skinToneSupport = false},
	EmojiData{ .emoji = "🏙️", .name = "cityscape", .group = GRP(4), .keywords = EKW_898, .skinToneSupport = false},
	EmojiData{ .emoji = "🌄", .name = "sunrise over mountains", .group = GRP(4), .keywords = EKW_899, .skinToneSupport = false},
	EmojiData{ .emoji = "🌅", .name = "sunrise", .group = GRP(4), .keywords = EKW_900, .skinToneSupport = false},
	EmojiData{ .emoji = "🌆", .name = "cityscape at dusk", .group = GRP(4), .keywords = EKW_901, .skinToneSupport = false},
	EmojiData{ .emoji = "🌇", .name = "sunset", .group = GRP(4), .keywords = EKW_902, .skinToneSupport = false},
	EmojiData{ .emoji = "🌉", .name = "bridge at night", .group = GRP(4), .keywords = EKW_903, .skinToneSupport = false},
	EmojiData{ .emoji = "♨️", .name = "hot springs", .group = GRP(4), .keywords = EKW_904, .skinToneSupport = false},
	EmojiData{ .emoji = "🎠", .name = "carousel horse", .group = GRP(4), .keywords = EKW_905, .skinToneSupport = false},
	EmojiData{ .emoji = "🛝", .name = "playground slide", .group = GRP(4), .keywords = EKW_906, .skinToneSupport = false},
	EmojiData{ .emoji = "🎡", .name = "ferris wheel", .group = GRP(4), .keywords = EKW_907, .skinToneSupport = false},
	EmojiData{ .emoji = "🎢", .name = "roller coaster", .group = GRP(4), .keywords = EKW_908, .skinToneSupport = false},
	EmojiData{ .emoji = "💈", .name = "barber pole", .group = GRP(4), .keywords = EKW_909, .skinToneSupport = false},
	EmojiData{ .emoji = "🎪", .name = "circus tent", .group = GRP(4), .keywords = EKW_910, .skinToneSupport = false},
	EmojiData{ .emoji = "🚂", .name = "locomotive", .group = GRP(4), .keywords = EKW_911, .skinToneSupport = false},
	EmojiData{ .emoji = "🚃", .name = "railway car", .group = GRP(4), .keywords = EKW_912, .skinToneSupport = false},
	EmojiData{ .emoji = "🚄", .name = "high-speed train", .group = GRP(4), .keywords = EKW_913, .skinToneSupport = false},
	EmojiData{ .emoji = "🚅", .name = "bullet train", .group = GRP(4), .keywords = EKW_914, .skinToneSupport = false},
	EmojiData{ .emoji = "🚆", .name = "train", .group = GRP(4), .keywords = EKW_915, .skinToneSupport = false},
	EmojiData{ .emoji = "🚇", .name = "metro", .group = GRP(4), .keywords = EKW_916, .skinToneSupport = false},
	EmojiData{ .emoji = "🚈", .name = "light rail", .group = GRP(4), .keywords = EKW_917, .skinToneSupport = false},
	EmojiData{ .emoji = "🚉", .name = "station", .group = GRP(4), .keywords = EKW_918, .skinToneSupport = false},
	EmojiData{ .emoji = "🚊", .name = "tram", .group = GRP(4), .keywords = EKW_919, .skinToneSupport = false},
	EmojiData{ .emoji = "🚝", .name = "monorail", .group = GRP(4), .keywords = EKW_920, .skinToneSupport = false},
	EmojiData{ .emoji = "🚞", .name = "mountain railway", .group = GRP(4), .keywords = EKW_921, .skinToneSupport = false},
	EmojiData{ .emoji = "🚋", .name = "tram car", .group = GRP(4), .keywords = EKW_922, .skinToneSupport = false},
	EmojiData{ .emoji = "🚌", .name = "bus", .group = GRP(4), .keywords = EKW_923, .skinToneSupport = false},
	EmojiData{ .emoji = "🚍", .name = "oncoming bus", .group = GRP(4), .keywords = EKW_924, .skinToneSupport = false},
	EmojiData{ .emoji = "🚎", .name = "trolleybus", .group = GRP(4), .keywords = EKW_925, .skinToneSupport = false},
	EmojiData{ .emoji = "🚐", .name = "minibus", .group = GRP(4), .keywords = EKW_926, .skinToneSupport = false},
	EmojiData{ .emoji = "🚑", .name = "ambulance", .group = GRP(4), .keywords = EKW_927, .skinToneSupport = false},
	EmojiData{ .emoji = "🚒", .name = "fire engine", .group = GRP(4), .keywords = EKW_928, .skinToneSupport = false},
	EmojiData{ .emoji = "🚓", .name = "police car", .group = GRP(4), .keywords = EKW_929, .skinToneSupport = false},
	EmojiData{ .emoji = "🚔", .name = "oncoming police car", .group = GRP(4), .keywords = EKW_930, .skinToneSupport = false},
	EmojiData{ .emoji = "🚕", .name = "taxi", .group = GRP(4), .keywords = EKW_931, .skinToneSupport = false},
	EmojiData{ .emoji = "🚖", .name = "oncoming taxi", .group = GRP(4), .keywords = EKW_932, .skinToneSupport = false},
	EmojiData{ .emoji = "🚗", .name = "automobile", .group = GRP(4), .keywords = EKW_933, .skinToneSupport = false},
	EmojiData{ .emoji = "🚘", .name = "oncoming automobile", .group = GRP(4), .keywords = EKW_934, .skinToneSupport = false},
	EmojiData{ .emoji = "🚙", .name = "sport utility vehicle", .group = GRP(4), .keywords = EKW_935, .skinToneSupport = false},
	EmojiData{ .emoji = "🛻", .name = "pickup truck", .group = GRP(4), .keywords = EKW_936, .skinToneSupport = false},
	EmojiData{ .emoji = "🚚", .name = "delivery truck", .group = GRP(4), .keywords = EKW_937, .skinToneSupport = false},
	EmojiData{ .emoji = "🚛", .name = "articulated lorry", .group = GRP(4), .keywords = EKW_938, .skinToneSupport = false},
	EmojiData{ .emoji = "🚜", .name = "tractor", .group = GRP(4), .keywords = EKW_939, .skinToneSupport = false},
	EmojiData{ .emoji = "🏎️", .name = "racing car", .group = GRP(4), .keywords = EKW_940, .skinToneSupport = false},
	EmojiData{ .emoji = "🏍️", .name = "motorcycle", .group = GRP(4), .keywords = EKW_941, .skinToneSupport = false},
	EmojiData{ .emoji = "🛵", .name = "motor scooter", .group = GRP(4), .keywords = EKW_942, .skinToneSupport = false},
	EmojiData{ .emoji = "🦽", .name = "manual wheelchair", .group = GRP(4), .keywords = EKW_943, .skinToneSupport = false},
	EmojiData{ .emoji = "🦼", .name = "motorized wheelchair", .group = GRP(4), .keywords = EKW_944, .skinToneSupport = false},
	EmojiData{ .emoji = "🛺", .name = "auto rickshaw", .group = GRP(4), .keywords = EKW_945, .skinToneSupport = false},
	EmojiData{ .emoji = "🚲", .name = "bicycle", .group = GRP(4), .keywords = EKW_946, .skinToneSupport = false},
	EmojiData{ .emoji = "🛴", .name = "kick scooter", .group = GRP(4), .keywords = EKW_947, .skinToneSupport = false},
	EmojiData{ .emoji = "🛹", .name = "skateboard", .group = GRP(4), .keywords = EKW_948, .skinToneSupport = false},
	EmojiData{ .emoji = "🛼", .name = "roller skate", .group = GRP(4), .keywords = EKW_949, .skinToneSupport = false},
	EmojiData{ .emoji = "🚏", .name = "bus stop", .group = GRP(4), .keywords = EKW_950, .skinToneSupport = false},
	EmojiData{ .emoji = "🛣️", .name = "motorway", .group = GRP(4), .keywords = EKW_951, .skinToneSupport = false},
	EmojiData{ .emoji = "🛤️", .name = "railway track", .group = GRP(4), .keywords = EKW_952, .skinToneSupport = false},
	EmojiData{ .emoji = "🛢️", .name = "oil drum", .group = GRP(4), .keywords = EKW_953, .skinToneSupport = false},
	EmojiData{ .emoji = "⛽", .name = "fuel pump", .group = GRP(4), .keywords = EKW_954, .skinToneSupport = false},
	EmojiData{ .emoji = "🛞", .name = "wheel", .group = GRP(4), .keywords = EKW_955, .skinToneSupport = false},
	EmojiData{ .emoji = "🚨", .name = "police car light", .group = GRP(4), .keywords = EKW_956, .skinToneSupport = false},
	EmojiData{ .emoji = "🚥", .name = "horizontal traffic light", .group = GRP(4), .keywords = EKW_957, .skinToneSupport = false},
	EmojiData{ .emoji = "🚦", .name = "vertical traffic light", .group = GRP(4), .keywords = EKW_958, .skinToneSupport = false},
	EmojiData{ .emoji = "🛑", .name = "stop sign", .group = GRP(4), .keywords = EKW_959, .skinToneSupport = false},
	EmojiData{ .emoji = "🚧", .name = "construction", .group = GRP(4), .keywords = EKW_960, .skinToneSupport = false},
	EmojiData{ .emoji = "⚓", .name = "anchor", .group = GRP(4), .keywords = EKW_961, .skinToneSupport = false},
	EmojiData{ .emoji = "🛟", .name = "ring buoy", .group = GRP(4), .keywords = EKW_962, .skinToneSupport = false},
	EmojiData{ .emoji = "⛵", .name = "sailboat", .group = GRP(4), .keywords = EKW_963, .skinToneSupport = false},
	EmojiData{ .emoji = "🛶", .name = "canoe", .group = GRP(4), .keywords = EKW_964, .skinToneSupport = false},
	EmojiData{ .emoji = "🚤", .name = "speedboat", .group = GRP(4), .keywords = EKW_965, .skinToneSupport = false},
	EmojiData{ .emoji = "🛳️", .name = "passenger ship", .group = GRP(4), .keywords = EKW_966, .skinToneSupport = false},
	EmojiData{ .emoji = "⛴️", .name = "ferry", .group = GRP(4), .keywords = EKW_967, .skinToneSupport = false},
	EmojiData{ .emoji = "🛥️", .name = "motor boat", .group = GRP(4), .keywords = EKW_968, .skinToneSupport = false},
	EmojiData{ .emoji = "🚢", .name = "ship", .group = GRP(4), .keywords = EKW_969, .skinToneSupport = false},
	EmojiData{ .emoji = "✈️", .name = "airplane", .group = GRP(4), .keywords = EKW_970, .skinToneSupport = false},
	EmojiData{ .emoji = "🛩️", .name = "small airplane", .group = GRP(4), .keywords = EKW_971, .skinToneSupport = false},
	EmojiData{ .emoji = "🛫", .name = "airplane departure", .group = GRP(4), .keywords = EKW_972, .skinToneSupport = false},
	EmojiData{ .emoji = "🛬", .name = "airplane arrival", .group = GRP(4), .keywords = EKW_973, .skinToneSupport = false},
	EmojiData{ .emoji = "🪂", .name = "parachute", .group = GRP(4), .keywords = EKW_974, .skinToneSupport = false},
	EmojiData{ .emoji = "💺", .name = "seat", .group = GRP(4), .keywords = EKW_975, .skinToneSupport = false},
	EmojiData{ .emoji = "🚁", .name = "helicopter", .group = GRP(4), .keywords = EKW_976, .skinToneSupport = false},
	EmojiData{ .emoji = "🚟", .name = "suspension railway", .group = GRP(4), .keywords = EKW_977, .skinToneSupport = false},
	EmojiData{ .emoji = "🚠", .name = "mountain cableway", .group = GRP(4), .keywords = EKW_978, .skinToneSupport = false},
	EmojiData{ .emoji = "🚡", .name = "aerial tramway", .group = GRP(4), .keywords = EKW_979, .skinToneSupport = false},
	EmojiData{ .emoji = "🛰️", .name = "satellite", .group = GRP(4), .keywords = EKW_980, .skinToneSupport = false},
	EmojiData{ .emoji = "🚀", .name = "rocket", .group = GRP(4), .keywords = EKW_981, .skinToneSupport = false},
	EmojiData{ .emoji = "🛸", .name = "flying saucer", .group = GRP(4), .keywords = EKW_982, .skinToneSupport = false},
	EmojiData{ .emoji = "🛎️", .name = "bellhop bell", .group = GRP(4), .keywords = EKW_983, .skinToneSupport = false},
	EmojiData{ .emoji = "🧳", .name = "luggage", .group = GRP(4), .keywords = EKW_984, .skinToneSupport = false},
	EmojiData{ .emoji = "⌛", .name = "hourglass done", .group = GRP(4), .keywords = EKW_985, .skinToneSupport = false},
	EmojiData{ .emoji = "⏳", .name = "hourglass not done", .group = GRP(4), .keywords = EKW_986, .skinToneSupport = false},
	EmojiData{ .emoji = "⌚", .name = "watch", .group = GRP(4), .keywords = EKW_987, .skinToneSupport = false},
	EmojiData{ .emoji = "⏰", .name = "alarm clock", .group = GRP(4), .keywords = EKW_988, .skinToneSupport = false},
	EmojiData{ .emoji = "⏱️", .name = "stopwatch", .group = GRP(4), .keywords = EKW_989, .skinToneSupport = false},
	EmojiData{ .emoji = "⏲️", .name = "timer clock", .group = GRP(4), .keywords = EKW_990, .skinToneSupport = false},
	EmojiData{ .emoji = "🕰️", .name = "mantelpiece clock", .group = GRP(4), .keywords = EKW_991, .skinToneSupport = false},
	EmojiData{ .emoji = "🕛", .name = "twelve o’clock", .group = GRP(4), .keywords = EKW_992, .skinToneSupport = false},
	EmojiData{ .emoji = "🕧", .name = "twelve-thirty", .group = GRP(4), .keywords = EKW_993, .skinToneSupport = false},
	EmojiData{ .emoji = "🕐", .name = "one o’clock", .group = GRP(4), .keywords = EKW_994, .skinToneSupport = false},
	EmojiData{ .emoji = "🕜", .name = "one-thirty", .group = GRP(4), .keywords = EKW_995, .skinToneSupport = false},
	EmojiData{ .emoji = "🕑", .name = "two o’clock", .group = GRP(4), .keywords = EKW_996, .skinToneSupport = false},
	EmojiData{ .emoji = "🕝", .name = "two-thirty", .group = GRP(4), .keywords = EKW_997, .skinToneSupport = false},
	EmojiData{ .emoji = "🕒", .name = "three o’clock", .group = GRP(4), .keywords = EKW_998, .skinToneSupport = false},
	EmojiData{ .emoji = "🕞", .name = "three-thirty", .group = GRP(4), .keywords = EKW_999, .skinToneSupport = false},
	EmojiData{ .emoji = "🕓", .name = "four o’clock", .group = GRP(4), .keywords = EKW_1000, .skinToneSupport = false},
	EmojiData{ .emoji = "🕟", .name = "four-thirty", .group = GRP(4), .keywords = EKW_1001, .skinToneSupport = false},
	EmojiData{ .emoji = "🕔", .name = "five o’clock", .group = GRP(4), .keywords = EKW_1002, .skinToneSupport = false},
	EmojiData{ .emoji = "🕠", .name = "five-thirty", .group = GRP(4), .keywords = EKW_1003, .skinToneSupport = false},
	EmojiData{ .emoji = "🕕", .name = "six o’clock", .group = GRP(4), .keywords = EKW_1004, .skinToneSupport = false},
	EmojiData{ .emoji = "🕡", .name = "six-thirty", .group = GRP(4), .keywords = EKW_1005, .skinToneSupport = false},
	EmojiData{ .emoji = "🕖", .name = "seven o’clock", .group = GRP(4), .keywords = EKW_1006, .skinToneSupport = false},
	EmojiData{ .emoji = "🕢", .name = "seven-thirty", .group = GRP(4), .keywords = EKW_1007, .skinToneSupport = false},
	EmojiData{ .emoji = "🕗", .name = "eight o’clock", .group = GRP(4), .keywords = EKW_1008, .skinToneSupport = false},
	EmojiData{ .emoji = "🕣", .name = "eight-thirty", .group = GRP(4), .keywords = EKW_1009, .skinToneSupport = false},
	EmojiData{ .emoji = "🕘", .name = "nine o’clock", .group = GRP(4), .keywords = EKW_1010, .skinToneSupport = false},
	EmojiData{ .emoji = "🕤", .name = "nine-thirty", .group = GRP(4), .keywords = EKW_1011, .skinToneSupport = false},
	EmojiData{ .emoji = "🕙", .name = "ten o’clock", .group = GRP(4), .keywords = EKW_1012, .skinToneSupport = false},
	EmojiData{ .emoji = "🕥", .name = "ten-thirty", .group = GRP(4), .keywords = EKW_1013, .skinToneSupport = false},
	EmojiData{ .emoji = "🕚", .name = "eleven o’clock", .group = GRP(4), .keywords = EKW_1014, .skinToneSupport = false},
	EmojiData{ .emoji = "🕦", .name = "eleven-thirty", .group = GRP(4), .keywords = EKW_1015, .skinToneSupport = false},
	EmojiData{ .emoji = "🌑", .name = "new moon", .group = GRP(4), .keywords = EKW_1016, .skinToneSupport = false},
	EmojiData{ .emoji = "🌒", .name = "waxing crescent moon", .group = GRP(4), .keywords = EKW_1017, .skinToneSupport = false},
	EmojiData{ .emoji = "🌓", .name = "first quarter moon", .group = GRP(4), .keywords = EKW_1018, .skinToneSupport = false},
	EmojiData{ .emoji = "🌔", .name = "waxing gibbous moon", .group = GRP(4), .keywords = EKW_1019, .skinToneSupport = false},
	EmojiData{ .emoji = "🌕", .name = "full moon", .group = GRP(4), .keywords = EKW_1020, .skinToneSupport = false},
	EmojiData{ .emoji = "🌖", .name = "waning gibbous moon", .group = GRP(4), .keywords = EKW_1021, .skinToneSupport = false},
	EmojiData{ .emoji = "🌗", .name = "last quarter moon", .group = GRP(4), .keywords = EKW_1022, .skinToneSupport = false},
	EmojiData{ .emoji = "🌘", .name = "waning crescent moon", .group = GRP(4), .keywords = EKW_1023, .skinToneSupport = false},
	EmojiData{ .emoji = "🌙", .name = "crescent moon", .group = GRP(4), .keywords = EKW_1024, .skinToneSupport = false},
	EmojiData{ .emoji = "🌚", .name = "new moon face", .group = GRP(4), .keywords = EKW_1025, .skinToneSupport = false},
	EmojiData{ .emoji = "🌛", .name = "first quarter moon face", .group = GRP(4), .keywords = EKW_1026, .skinToneSupport = false},
	EmojiData{ .emoji = "🌜", .name = "last quarter moon face", .group = GRP(4), .keywords = EKW_1027, .skinToneSupport = false},
	EmojiData{ .emoji = "🌡️", .name = "thermometer", .group = GRP(4), .keywords = EKW_1028, .skinToneSupport = false},
	EmojiData{ .emoji = "☀️", .name = "sun", .group = GRP(4), .keywords = EKW_1029, .skinToneSupport = false},
	EmojiData{ .emoji = "🌝", .name = "full moon face", .group = GRP(4), .keywords = EKW_1030, .skinToneSupport = false},
	EmojiData{ .emoji = "🌞", .name = "sun with face", .group = GRP(4), .keywords = EKW_1031, .skinToneSupport = false},
	EmojiData{ .emoji = "🪐", .name = "ringed planet", .group = GRP(4), .keywords = EKW_1032, .skinToneSupport = false},
	EmojiData{ .emoji = "⭐", .name = "star", .group = GRP(4), .keywords = EKW_1033, .skinToneSupport = false},
	EmojiData{ .emoji = "🌟", .name = "glowing star", .group = GRP(4), .keywords = EKW_1034, .skinToneSupport = false},
	EmojiData{ .emoji = "🌠", .name = "shooting star", .group = GRP(4), .keywords = EKW_1035, .skinToneSupport = false},
	EmojiData{ .emoji = "🌌", .name = "milky way", .group = GRP(4), .keywords = EKW_1036, .skinToneSupport = false},
	EmojiData{ .emoji = "☁️", .name = "cloud", .group = GRP(4), .keywords = EKW_1037, .skinToneSupport = false},
	EmojiData{ .emoji = "⛅", .name = "sun behind cloud", .group = GRP(4), .keywords = EKW_1038, .skinToneSupport = false},
	EmojiData{ .emoji = "⛈️", .name = "cloud with lightning and rain", .group = GRP(4), .keywords = EKW_1039, .skinToneSupport = false},
	EmojiData{ .emoji = "🌤️", .name = "sun behind small cloud", .group = GRP(4), .keywords = EKW_1040, .skinToneSupport = false},
	EmojiData{ .emoji = "🌥️", .name = "sun behind large cloud", .group = GRP(4), .keywords = EKW_1041, .skinToneSupport = false},
	EmojiData{ .emoji = "🌦️", .name = "sun behind rain cloud", .group = GRP(4), .keywords = EKW_1042, .skinToneSupport = false},
	EmojiData{ .emoji = "🌧️", .name = "cloud with rain", .group = GRP(4), .keywords = EKW_1043, .skinToneSupport = false},
	EmojiData{ .emoji = "🌨️", .name = "cloud with snow", .group = GRP(4), .keywords = EKW_1044, .skinToneSupport = false},
	EmojiData{ .emoji = "🌩️", .name = "cloud with lightning", .group = GRP(4), .keywords = EKW_1045, .skinToneSupport = false},
	EmojiData{ .emoji = "🌪️", .name = "tornado", .group = GRP(4), .keywords = EKW_1046, .skinToneSupport = false},
	EmojiData{ .emoji = "🌫️", .name = "fog", .group = GRP(4), .keywords = EKW_1047, .skinToneSupport = false},
	EmojiData{ .emoji = "🌬️", .name = "wind face", .group = GRP(4), .keywords = EKW_1048, .skinToneSupport = false},
	EmojiData{ .emoji = "🌀", .name = "cyclone", .group = GRP(4), .keywords = EKW_1049, .skinToneSupport = false},
	EmojiData{ .emoji = "🌈", .name = "rainbow", .group = GRP(4), .keywords = EKW_1050, .skinToneSupport = false},
	EmojiData{ .emoji = "🌂", .name = "closed umbrella", .group = GRP(4), .keywords = EKW_1051, .skinToneSupport = false},
	EmojiData{ .emoji = "☂️", .name = "umbrella", .group = GRP(4), .keywords = EKW_1052, .skinToneSupport = false},
	EmojiData{ .emoji = "☔", .name = "umbrella with rain drops", .group = GRP(4), .keywords = EKW_1053, .skinToneSupport = false},
	EmojiData{ .emoji = "⛱️", .name = "umbrella on ground", .group = GRP(4), .keywords = EKW_1054, .skinToneSupport = false},
	EmojiData{ .emoji = "⚡", .name = "high voltage", .group = GRP(4), .keywords = EKW_1055, .skinToneSupport = false},
	EmojiData{ .emoji = "❄️", .name = "snowflake", .group = GRP(4), .keywords = EKW_1056, .skinToneSupport = false},
	EmojiData{ .emoji = "☃️", .name = "snowman", .group = GRP(4), .keywords = EKW_1057, .skinToneSupport = false},
	EmojiData{ .emoji = "⛄", .name = "snowman without snow", .group = GRP(4), .keywords = EKW_1058, .skinToneSupport = false},
	EmojiData{ .emoji = "☄️", .name = "comet", .group = GRP(4), .keywords = EKW_1059, .skinToneSupport = false},
	EmojiData{ .emoji = "🔥", .name = "fire", .group = GRP(4), .keywords = EKW_1060, .skinToneSupport = false},
	EmojiData{ .emoji = "💧", .name = "droplet", .group = GRP(4), .keywords = EKW_1061, .skinToneSupport = false},
	EmojiData{ .emoji = "🌊", .name = "water wave", .group = GRP(4), .keywords = EKW_1062, .skinToneSupport = false},
	EmojiData{ .emoji = "🎃", .name = "jack-o-lantern", .group = GRP(5), .keywords = EKW_1063, .skinToneSupport = false},
	EmojiData{ .emoji = "🎄", .name = "Christmas tree", .group = GRP(5), .keywords = EKW_1064, .skinToneSupport = false},
	EmojiData{ .emoji = "🎆", .name = "fireworks", .group = GRP(5), .keywords = EKW_1065, .skinToneSupport = false},
	EmojiData{ .emoji = "🎇", .name = "sparkler", .group = GRP(5), .keywords = EKW_1066, .skinToneSupport = false},
	EmojiData{ .emoji = "🧨", .name = "firecracker", .group = GRP(5), .keywords = EKW_1067, .skinToneSupport = false},
	EmojiData{ .emoji = "✨", .name = "sparkles", .group = GRP(5), .keywords = EKW_1068, .skinToneSupport = false},
	EmojiData{ .emoji = "🎈", .name = "balloon", .group = GRP(5), .keywords = EKW_1069, .skinToneSupport = false},
	EmojiData{ .emoji = "🎉", .name = "party popper", .group = GRP(5), .keywords = EKW_1070, .skinToneSupport = false},
	EmojiData{ .emoji = "🎊", .name = "confetti ball", .group = GRP(5), .keywords = EKW_1071, .skinToneSupport = false},
	EmojiData{ .emoji = "🎋", .name = "tanabata tree", .group = GRP(5), .keywords = EKW_1072, .skinToneSupport = false},
	EmojiData{ .emoji = "🎍", .name = "pine decoration", .group = GRP(5), .keywords = EKW_1073, .skinToneSupport = false},
	EmojiData{ .emoji = "🎎", .name = "Japanese dolls", .group = GRP(5), .keywords = EKW_1074, .skinToneSupport = false},
	EmojiData{ .emoji = "🎏", .name = "carp streamer", .group = GRP(5), .keywords = EKW_1075, .skinToneSupport = false},
	EmojiData{ .emoji = "🎐", .name = "wind chime", .group = GRP(5), .keywords = EKW_1076, .skinToneSupport = false},
	EmojiData{ .emoji = "🎑", .name = "moon viewing ceremony", .group = GRP(5), .keywords = EKW_1077, .skinToneSupport = false},
	EmojiData{ .emoji = "🧧", .name = "red envelope", .group = GRP(5), .keywords = EKW_1078, .skinToneSupport = false},
	EmojiData{ .emoji = "🎀", .name = "ribbon", .group = GRP(5), .keywords = EKW_1079, .skinToneSupport = false},
	EmojiData{ .emoji = "🎁", .name = "wrapped gift", .group = GRP(5), .keywords = EKW_1080, .skinToneSupport = false},
	EmojiData{ .emoji = "🎗️", .name = "reminder ribbon", .group = GRP(5), .keywords = EKW_1081, .skinToneSupport = false},
	EmojiData{ .emoji = "🎟️", .name = "admission tickets", .group = GRP(5), .keywords = EKW_1082, .skinToneSupport = false},
	EmojiData{ .emoji = "🎫", .name = "ticket", .group = GRP(5), .keywords = EKW_1083, .skinToneSupport = false},
	EmojiData{ .emoji = "🎖️", .name = "military medal", .group = GRP(5), .keywords = EKW_1084, .skinToneSupport = false},
	EmojiData{ .emoji = "🏆", .name = "trophy", .group = GRP(5), .keywords = EKW_1085, .skinToneSupport = false},
	EmojiData{ .emoji = "🏅", .name = "sports medal", .group = GRP(5), .keywords = EKW_1086, .skinToneSupport = false},
	EmojiData{ .emoji = "🥇", .name = "1st place medal", .group = GRP(5), .keywords = EKW_1087, .skinToneSupport = false},
	EmojiData{ .emoji = "🥈", .name = "2nd place medal", .group = GRP(5), .keywords = EKW_1088, .skinToneSupport = false},
	EmojiData{ .emoji = "🥉", .name = "3rd place medal", .group = GRP(5), .keywords = EKW_1089, .skinToneSupport = false},
	EmojiData{ .emoji = "⚽", .name = "soccer ball", .group = GRP(5), .keywords = EKW_1090, .skinToneSupport = false},
	EmojiData{ .emoji = "⚾", .name = "baseball", .group = GRP(5), .keywords = EKW_1091, .skinToneSupport = false},
	EmojiData{ .emoji = "🥎", .name = "softball", .group = GRP(5), .keywords = EKW_1092, .skinToneSupport = false},
	EmojiData{ .emoji = "🏀", .name = "basketball", .group = GRP(5), .keywords = EKW_1093, .skinToneSupport = false},
	EmojiData{ .emoji = "🏐", .name = "volleyball", .group = GRP(5), .keywords = EKW_1094, .skinToneSupport = false},
	EmojiData{ .emoji = "🏈", .name = "american football", .group = GRP(5), .keywords = EKW_1095, .skinToneSupport = false},
	EmojiData{ .emoji = "🏉", .name = "rugby football", .group = GRP(5), .keywords = EKW_1096, .skinToneSupport = false},
	EmojiData{ .emoji = "🎾", .name = "tennis", .group = GRP(5), .keywords = EKW_1097, .skinToneSupport = false},
	EmojiData{ .emoji = "🥏", .name = "flying disc", .group = GRP(5), .keywords = EKW_1098, .skinToneSupport = false},
	EmojiData{ .emoji = "🎳", .name = "bowling", .group = GRP(5), .keywords = EKW_1099, .skinToneSupport = false},
	EmojiData{ .emoji = "🏏", .name = "cricket game", .group = GRP(5), .keywords = EKW_1100, .skinToneSupport = false},
	EmojiData{ .emoji = "🏑", .name = "field hockey", .group = GRP(5), .keywords = EKW_1101, .skinToneSupport = false},
	EmojiData{ .emoji = "🏒", .name = "ice hockey", .group = GRP(5), .keywords = EKW_1102, .skinToneSupport = false},
	EmojiData{ .emoji = "🥍", .name = "lacrosse", .group = GRP(5), .keywords = EKW_1103, .skinToneSupport = false},
	EmojiData{ .emoji = "🏓", .name = "ping pong", .group = GRP(5), .keywords = EKW_1104, .skinToneSupport = false},
	EmojiData{ .emoji = "🏸", .name = "badminton", .group = GRP(5), .keywords = EKW_1105, .skinToneSupport = false},
	EmojiData{ .emoji = "🥊", .name = "boxing glove", .group = GRP(5), .keywords = EKW_1106, .skinToneSupport = false},
	EmojiData{ .emoji = "🥋", .name = "martial arts uniform", .group = GRP(5), .keywords = EKW_1107, .skinToneSupport = false},
	EmojiData{ .emoji = "🥅", .name = "goal net", .group = GRP(5), .keywords = EKW_1108, .skinToneSupport = false},
	EmojiData{ .emoji = "⛳", .name = "flag in hole", .group = GRP(5), .keywords = EKW_1109, .skinToneSupport = false},
	EmojiData{ .emoji = "⛸️", .name = "ice skate", .group = GRP(5), .keywords = EKW_1110, .skinToneSupport = false},
	EmojiData{ .emoji = "🎣", .name = "fishing pole", .group = GRP(5), .keywords = EKW_1111, .skinToneSupport = false},
	EmojiData{ .emoji = "🤿", .name = "diving mask", .group = GRP(5), .keywords = EKW_1112, .skinToneSupport = false},
	EmojiData{ .emoji = "🎽", .name = "running shirt", .group = GRP(5), .keywords = EKW_1113, .skinToneSupport = false},
	EmojiData{ .emoji = "🎿", .name = "skis", .group = GRP(5), .keywords = EKW_1114, .skinToneSupport = false},
	EmojiData{ .emoji = "🛷", .name = "sled", .group = GRP(5), .keywords = EKW_1115, .skinToneSupport = false},
	EmojiData{ .emoji = "🥌", .name = "curling stone", .group = GRP(5), .keywords = EKW_1116, .skinToneSupport = false},
	EmojiData{ .emoji = "🎯", .name = "bullseye", .group = GRP(5), .keywords = EKW_1117, .skinToneSupport = false},
	EmojiData{ .emoji = "🪀", .name = "yo-yo", .group = GRP(5), .keywords = EKW_1118, .skinToneSupport = false},
	EmojiData{ .emoji = "🪁", .name = "kite", .group = GRP(5), .keywords = EKW_1119, .skinToneSupport = false},
	EmojiData{ .emoji = "🔫", .name = "water pistol", .group = GRP(5), .keywords = EKW_1120, .skinToneSupport = false},
	EmojiData{ .emoji = "🎱", .name = "pool 8 ball", .group = GRP(5), .keywords = EKW_1121, .skinToneSupport = false},
	EmojiData{ .emoji = "🔮", .name = "crystal ball", .group = GRP(5), .keywords = EKW_1122, .skinToneSupport = false},
	EmojiData{ .emoji = "🪄", .name = "magic wand", .group = GRP(5), .keywords = EKW_1123, .skinToneSupport = false},
	EmojiData{ .emoji = "🎮", .name = "video game", .group = GRP(5), .keywords = EKW_1124, .skinToneSupport = false},
	EmojiData{ .emoji = "🕹️", .name = "joystick", .group = GRP(5), .keywords = EKW_1125, .skinToneSupport = false},
	EmojiData{ .emoji = "🎰", .name = "slot machine", .group = GRP(5), .keywords = EKW_1126, .skinToneSupport = false},
	EmojiData{ .emoji = "🎲", .name = "game die", .group = GRP(5), .keywords = EKW_1127, .skinToneSupport = false},
	EmojiData{ .emoji = "🧩", .name = "puzzle piece", .group = GRP(5), .keywords = EKW_1128, .skinToneSupport = false},
	EmojiData{ .emoji = "🧸", .name = "teddy bear", .group = GRP(5), .keywords = EKW_1129, .skinToneSupport = false},
	EmojiData{ .emoji = "🪅", .name = "piñata", .group = GRP(5), .keywords = EKW_1130, .skinToneSupport = false},
	EmojiData{ .emoji = "🪩", .name = "mirror ball", .group = GRP(5), .keywords = EKW_1131, .skinToneSupport = false},
	EmojiData{ .emoji = "🪆", .name = "nesting dolls", .group = GRP(5), .keywords = EKW_1132, .skinToneSupport = false},
	EmojiData{ .emoji = "♠️", .name = "spade suit", .group = GRP(5), .keywords = EKW_1133, .skinToneSupport = false},
	EmojiData{ .emoji = "♥️", .name = "heart suit", .group = GRP(5), .keywords = EKW_1134, .skinToneSupport = false},
	EmojiData{ .emoji = "♦️", .name = "diamond suit", .group = GRP(5), .keywords = EKW_1135, .skinToneSupport = false},
	EmojiData{ .emoji = "♣️", .name = "club suit", .group = GRP(5), .keywords = EKW_1136, .skinToneSupport = false},
	EmojiData{ .emoji = "♟️", .name = "chess pawn", .group = GRP(5), .keywords = EKW_1137, .skinToneSupport = false},
	EmojiData{ .emoji = "🃏", .name = "joker", .group = GRP(5), .keywords = EKW_1138, .skinToneSupport = false},
	EmojiData{ .emoji = "🀄", .name = "mahjong red dragon", .group = GRP(5), .keywords = EKW_1139, .skinToneSupport = false},
	EmojiData{ .emoji = "🎴", .name = "flower playing cards", .group = GRP(5), .keywords = EKW_1140, .skinToneSupport = false},
	EmojiData{ .emoji = "🎭", .name = "performing arts", .group = GRP(5), .keywords = EKW_1141, .skinToneSupport = false},
	EmojiData{ .emoji = "🖼️", .name = "framed picture", .group = GRP(5), .keywords = EKW_1142, .skinToneSupport = false},
	EmojiData{ .emoji = "🎨", .name = "artist palette", .group = GRP(5), .keywords = EKW_1143, .skinToneSupport = false},
	EmojiData{ .emoji = "🧵", .name = "thread", .group = GRP(5), .keywords = EKW_1144, .skinToneSupport = false},
	EmojiData{ .emoji = "🪡", .name = "sewing needle", .group = GRP(5), .keywords = EKW_1145, .skinToneSupport = false},
	EmojiData{ .emoji = "🧶", .name = "yarn", .group = GRP(5), .keywords = EKW_1146, .skinToneSupport = false},
	EmojiData{ .emoji = "🪢", .name = "knot", .group = GRP(5), .keywords = EKW_1147, .skinToneSupport = false},
	EmojiData{ .emoji = "👓", .name = "glasses", .group = GRP(6), .keywords = EKW_1148, .skinToneSupport = false},
	EmojiData{ .emoji = "🕶️", .name = "sunglasses", .group = GRP(6), .keywords = EKW_1149, .skinToneSupport = false},
	EmojiData{ .emoji = "🥽", .name = "goggles", .group = GRP(6), .keywords = EKW_1150, .skinToneSupport = false},
	EmojiData{ .emoji = "🥼", .name = "lab coat", .group = GRP(6), .keywords = EKW_1151, .skinToneSupport = false},
	EmojiData{ .emoji = "🦺", .name = "safety vest", .group = GRP(6), .keywords = EKW_1152, .skinToneSupport = false},
	EmojiData{ .emoji = "👔", .name = "necktie", .group = GRP(6), .keywords = EKW_1153, .skinToneSupport = false},
	EmojiData{ .emoji = "👕", .name = "t-shirt", .group = GRP(6), .keywords = EKW_1154, .skinToneSupport = false},
	EmojiData{ .emoji = "👖", .name = "jeans", .group = GRP(6), .keywords = EKW_1155, .skinToneSupport = false},
	EmojiData{ .emoji = "🧣", .name = "scarf", .group = GRP(6), .keywords = EKW_1156, .skinToneSupport = false},
	EmojiData{ .emoji = "🧤", .name = "gloves", .group = GRP(6), .keywords = EKW_1157, .skinToneSupport = false},
	EmojiData{ .emoji = "🧥", .name = "coat", .group = GRP(6), .keywords = EKW_1158, .skinToneSupport = false},
	EmojiData{ .emoji = "🧦", .name = "socks", .group = GRP(6), .keywords = EKW_1159, .skinToneSupport = false},
	EmojiData{ .emoji = "👗", .name = "dress", .group = GRP(6), .keywords = EKW_1160, .skinToneSupport = false},
	EmojiData{ .emoji = "👘", .name = "kimono", .group = GRP(6), .keywords = EKW_1161, .skinToneSupport = false},
	EmojiData{ .emoji = "🥻", .name = "sari", .group = GRP(6), .keywords = EKW_1162, .skinToneSupport = false},
	EmojiData{ .emoji = "🩱", .name = "one-piece swimsuit", .group = GRP(6), .keywords = EKW_1163, .skinToneSupport = false},
	EmojiData{ .emoji = "🩲", .name = "briefs", .group = GRP(6), .keywords = EKW_1164, .skinToneSupport = false},
	EmojiData{ .emoji = "🩳", .name = "shorts", .group = GRP(6), .keywords = EKW_1165, .skinToneSupport = false},
	EmojiData{ .emoji = "👙", .name = "bikini", .group = GRP(6), .keywords = EKW_1166, .skinToneSupport = false},
	EmojiData{ .emoji = "👚", .name = "woman’s clothes", .group = GRP(6), .keywords = EKW_1167, .skinToneSupport = false},
	EmojiData{ .emoji = "🪭", .name = "folding hand fan", .group = GRP(6), .keywords = EKW_1168, .skinToneSupport = false},
	EmojiData{ .emoji = "👛", .name = "purse", .group = GRP(6), .keywords = EKW_1169, .skinToneSupport = false},
	EmojiData{ .emoji = "👜", .name = "handbag", .group = GRP(6), .keywords = EKW_1170, .skinToneSupport = false},
	EmojiData{ .emoji = "👝", .name = "clutch bag", .group = GRP(6), .keywords = EKW_1171, .skinToneSupport = false},
	EmojiData{ .emoji = "🛍️", .name = "shopping bags", .group = GRP(6), .keywords = EKW_1172, .skinToneSupport = false},
	EmojiData{ .emoji = "🎒", .name = "backpack", .group = GRP(6), .keywords = EKW_1173, .skinToneSupport = false},
	EmojiData{ .emoji = "🩴", .name = "thong sandal", .group = GRP(6), .keywords = EKW_1174, .skinToneSupport = false},
	EmojiData{ .emoji = "👞", .name = "man’s shoe", .group = GRP(6), .keywords = EKW_1175, .skinToneSupport = false},
	EmojiData{ .emoji = "👟", .name = "running shoe", .group = GRP(6), .keywords = EKW_1176, .skinToneSupport = false},
	EmojiData{ .emoji = "🥾", .name = "hiking boot", .group = GRP(6), .keywords = EKW_1177, .skinToneSupport = false},
	EmojiData{ .emoji = "🥿", .name = "flat shoe", .group = GRP(6), .keywords = EKW_1178, .skinToneSupport = false},
	EmojiData{ .emoji = "👠", .name = "high-heeled shoe", .group = GRP(6), .keywords = EKW_1179, .skinToneSupport = false},
	EmojiData{ .emoji = "👡", .name = "woman’s sandal", .group = GRP(6), .keywords = EKW_1180, .skinToneSupport = false},
	EmojiData{ .emoji = "🩰", .name = "ballet shoes", .group = GRP(6), .keywords = EKW_1181, .skinToneSupport = false},
	EmojiData{ .emoji = "👢", .name = "woman’s boot", .group = GRP(6), .keywords = EKW_1182, .skinToneSupport = false},
	EmojiData{ .emoji = "🪮", .name = "hair pick", .group = GRP(6), .keywords = EKW_1183, .skinToneSupport = false},
	EmojiData{ .emoji = "👑", .name = "crown", .group = GRP(6), .keywords = EKW_1184, .skinToneSupport = false},
	EmojiData{ .emoji = "👒", .name = "woman’s hat", .group = GRP(6), .keywords = EKW_1185, .skinToneSupport = false},
	EmojiData{ .emoji = "🎩", .name = "top hat", .group = GRP(6), .keywords = EKW_1186, .skinToneSupport = false},
	EmojiData{ .emoji = "🎓", .name = "graduation cap", .group = GRP(6), .keywords = EKW_1187, .skinToneSupport = false},
	EmojiData{ .emoji = "🧢", .name = "billed cap", .group = GRP(6), .keywords = EKW_1188, .skinToneSupport = false},
	EmojiData{ .emoji = "🪖", .name = "military helmet", .group = GRP(6), .keywords = EKW_1189, .skinToneSupport = false},
	EmojiData{ .emoji = "⛑️", .name = "rescue worker’s helmet", .group = GRP(6), .keywords = EKW_1190, .skinToneSupport = false},
	EmojiData{ .emoji = "📿", .name = "prayer beads", .group = GRP(6), .keywords = EKW_1191, .skinToneSupport = false},
	EmojiData{ .emoji = "💄", .name = "lipstick", .group = GRP(6), .keywords = EKW_1192, .skinToneSupport = false},
	EmojiData{ .emoji = "💍", .name = "ring", .group = GRP(6), .keywords = EKW_1193, .skinToneSupport = false},
	EmojiData{ .emoji = "💎", .name = "gem stone", .group = GRP(6), .keywords = EKW_1194, .skinToneSupport = false},
	EmojiData{ .emoji = "🔇", .name = "muted speaker", .group = GRP(6), .keywords = EKW_1195, .skinToneSupport = false},
	EmojiData{ .emoji = "🔈", .name = "speaker low volume", .group = GRP(6), .keywords = EKW_1196, .skinToneSupport = false},
	EmojiData{ .emoji = "🔉", .name = "speaker medium volume", .group = GRP(6), .keywords = EKW_1197, .skinToneSupport = false},
	EmojiData{ .emoji = "🔊", .name = "speaker high volume", .group = GRP(6), .keywords = EKW_1198, .skinToneSupport = false},
	EmojiData{ .emoji = "📢", .name = "loudspeaker", .group = GRP(6), .keywords = EKW_1199, .skinToneSupport = false},
	EmojiData{ .emoji = "📣", .name = "megaphone", .group = GRP(6), .keywords = EKW_1200, .skinToneSupport = false},
	EmojiData{ .emoji = "📯", .name = "postal horn", .group = GRP(6), .keywords = EKW_1201, .skinToneSupport = false},
	EmojiData{ .emoji = "🔔", .name = "bell", .group = GRP(6), .keywords = EKW_1202, .skinToneSupport = false},
	EmojiData{ .emoji = "🔕", .name = "bell with slash", .group = GRP(6), .keywords = EKW_1203, .skinToneSupport = false},
	EmojiData{ .emoji = "🎼", .name = "musical score", .group = GRP(6), .keywords = EKW_1204, .skinToneSupport = false},
	EmojiData{ .emoji = "🎵", .name = "musical note", .group = GRP(6), .keywords = EKW_1205, .skinToneSupport = false},
	EmojiData{ .emoji = "🎶", .name = "musical notes", .group = GRP(6), .keywords = EKW_1206, .skinToneSupport = false},
	EmojiData{ .emoji = "🎙️", .name = "studio microphone", .group = GRP(6), .keywords = EKW_1207, .skinToneSupport = false},
	EmojiData{ .emoji = "🎚️", .name = "level slider", .group = GRP(6), .keywords = EKW_1208, .skinToneSupport = false},
	EmojiData{ .emoji = "🎛️", .name = "control knobs", .group = GRP(6), .keywords = EKW_1209, .skinToneSupport = false},
	EmojiData{ .emoji = "🎤", .name = "microphone", .group = GRP(6), .keywords = EKW_1210, .skinToneSupport = false},
	EmojiData{ .emoji = "🎧", .name = "headphone", .group = GRP(6), .keywords = EKW_1211, .skinToneSupport = false},
	EmojiData{ .emoji = "📻", .name = "radio", .group = GRP(6), .keywords = EKW_1212, .skinToneSupport = false},
	EmojiData{ .emoji = "🎷", .name = "saxophone", .group = GRP(6), .keywords = EKW_1213, .skinToneSupport = false},
	EmojiData{ .emoji = "🪗", .name = "accordion", .group = GRP(6), .keywords = EKW_1214, .skinToneSupport = false},
	EmojiData{ .emoji = "🎸", .name = "guitar", .group = GRP(6), .keywords = EKW_1215, .skinToneSupport = false},
	EmojiData{ .emoji = "🎹", .name = "musical keyboard", .group = GRP(6), .keywords = EKW_1216, .skinToneSupport = false},
	EmojiData{ .emoji = "🎺", .name = "trumpet", .group = GRP(6), .keywords = EKW_1217, .skinToneSupport = false},
	EmojiData{ .emoji = "🎻", .name = "violin", .group = GRP(6), .keywords = EKW_1218, .skinToneSupport = false},
	EmojiData{ .emoji = "🪕", .name = "banjo", .group = GRP(6), .keywords = EKW_1219, .skinToneSupport = false},
	EmojiData{ .emoji = "🥁", .name = "drum", .group = GRP(6), .keywords = EKW_1220, .skinToneSupport = false},
	EmojiData{ .emoji = "🪘", .name = "long drum", .group = GRP(6), .keywords = EKW_1221, .skinToneSupport = false},
	EmojiData{ .emoji = "🪇", .name = "maracas", .group = GRP(6), .keywords = EKW_1222, .skinToneSupport = false},
	EmojiData{ .emoji = "🪈", .name = "flute", .group = GRP(6), .keywords = EKW_1223, .skinToneSupport = false},
	EmojiData{ .emoji = "🪉", .name = "harp", .group = GRP(6), .keywords = EKW_1224, .skinToneSupport = false},
	EmojiData{ .emoji = "📱", .name = "mobile phone", .group = GRP(6), .keywords = EKW_1225, .skinToneSupport = false},
	EmojiData{ .emoji = "📲", .name = "mobile phone with arrow", .group = GRP(6), .keywords = EKW_1226, .skinToneSupport = false},
	EmojiData{ .emoji = "☎️", .name = "telephone", .group = GRP(6), .keywords = EKW_1227, .skinToneSupport = false},
	EmojiData{ .emoji = "📞", .name = "telephone receiver", .group = GRP(6), .keywords = EKW_1228, .skinToneSupport = false},
	EmojiData{ .emoji = "📟", .name = "pager", .group = GRP(6), .keywords = EKW_1229, .skinToneSupport = false},
	EmojiData{ .emoji = "📠", .name = "fax machine", .group = GRP(6), .keywords = EKW_1230, .skinToneSupport = false},
	EmojiData{ .emoji = "🔋", .name = "battery", .group = GRP(6), .keywords = EKW_1231, .skinToneSupport = false},
	EmojiData{ .emoji = "🪫", .name = "low battery", .group = GRP(6), .keywords = EKW_1232, .skinToneSupport = false},
	EmojiData{ .emoji = "🔌", .name = "electric plug", .group = GRP(6), .keywords = EKW_1233, .skinToneSupport = false},
	EmojiData{ .emoji = "💻", .name = "laptop", .group = GRP(6), .keywords = EKW_1234, .skinToneSupport = false},
	EmojiData{ .emoji = "🖥️", .name = "desktop computer", .group = GRP(6), .keywords = EKW_1235, .skinToneSupport = false},
	EmojiData{ .emoji = "🖨️", .name = "printer", .group = GRP(6), .keywords = EKW_1236, .skinToneSupport = false},
	EmojiData{ .emoji = "⌨️", .name = "keyboard", .group = GRP(6), .keywords = EKW_1237, .skinToneSupport = false},
	EmojiData{ .emoji = "🖱️", .name = "computer mouse", .group = GRP(6), .keywords = EKW_1238, .skinToneSupport = false},
	EmojiData{ .emoji = "🖲️", .name = "trackball", .group = GRP(6), .keywords = EKW_1239, .skinToneSupport = false},
	EmojiData{ .emoji = "💽", .name = "computer disk", .group = GRP(6), .keywords = EKW_1240, .skinToneSupport = false},
	EmojiData{ .emoji = "💾", .name = "floppy disk", .group = GRP(6), .keywords = EKW_1241, .skinToneSupport = false},
	EmojiData{ .emoji = "💿", .name = "optical disk", .group = GRP(6), .keywords = EKW_1242, .skinToneSupport = false},
	EmojiData{ .emoji = "📀", .name = "dvd", .group = GRP(6), .keywords = EKW_1243, .skinToneSupport = false},
	EmojiData{ .emoji = "🧮", .name = "abacus", .group = GRP(6), .keywords = EKW_1244, .skinToneSupport = false},
	EmojiData{ .emoji = "🎥", .name = "movie camera", .group = GRP(6), .keywords = EKW_1245, .skinToneSupport = false},
	EmojiData{ .emoji = "🎞️", .name = "film frames", .group = GRP(6), .keywords = EKW_1246, .skinToneSupport = false},
	EmojiData{ .emoji = "📽️", .name = "film projector", .group = GRP(6), .keywords = EKW_1247, .skinToneSupport = false},
	EmojiData{ .emoji = "🎬", .name = "clapper board", .group = GRP(6), .keywords = EKW_1248, .skinToneSupport = false},
	EmojiData{ .emoji = "📺", .name = "television", .group = GRP(6), .keywords = EKW_1249, .skinToneSupport = false},
	EmojiData{ .emoji = "📷", .name = "camera", .group = GRP(6), .keywords = EKW_1250, .skinToneSupport = false},
	EmojiData{ .emoji = "📸", .name = "camera with flash", .group = GRP(6), .keywords = EKW_1251, .skinToneSupport = false},
	EmojiData{ .emoji = "📹", .name = "video camera", .group = GRP(6), .keywords = EKW_1252, .skinToneSupport = false},
	EmojiData{ .emoji = "📼", .name = "videocassette", .group = GRP(6), .keywords = EKW_1253, .skinToneSupport = false},
	EmojiData{ .emoji = "🔍", .name = "magnifying glass tilted left", .group = GRP(6), .keywords = EKW_1254, .skinToneSupport = false},
	EmojiData{ .emoji = "🔎", .name = "magnifying glass tilted right", .group = GRP(6), .keywords = EKW_1255, .skinToneSupport = false},
	EmojiData{ .emoji = "🕯️", .name = "candle", .group = GRP(6), .keywords = EKW_1256, .skinToneSupport = false},
	EmojiData{ .emoji = "💡", .name = "light bulb", .group = GRP(6), .keywords = EKW_1257, .skinToneSupport = false},
	EmojiData{ .emoji = "🔦", .name = "flashlight", .group = GRP(6), .keywords = EKW_1258, .skinToneSupport = false},
	EmojiData{ .emoji = "🏮", .name = "red paper lantern", .group = GRP(6), .keywords = EKW_1259, .skinToneSupport = false},
	EmojiData{ .emoji = "🪔", .name = "diya lamp", .group = GRP(6), .keywords = EKW_1260, .skinToneSupport = false},
	EmojiData{ .emoji = "📔", .name = "notebook with decorative cover", .group = GRP(6), .keywords = EKW_1261, .skinToneSupport = false},
	EmojiData{ .emoji = "📕", .name = "closed book", .group = GRP(6), .keywords = EKW_1262, .skinToneSupport = false},
	EmojiData{ .emoji = "📖", .name = "open book", .group = GRP(6), .keywords = EKW_1263, .skinToneSupport = false},
	EmojiData{ .emoji = "📗", .name = "green book", .group = GRP(6), .keywords = EKW_1264, .skinToneSupport = false},
	EmojiData{ .emoji = "📘", .name = "blue book", .group = GRP(6), .keywords = EKW_1265, .skinToneSupport = false},
	EmojiData{ .emoji = "📙", .name = "orange book", .group = GRP(6), .keywords = EKW_1266, .skinToneSupport = false},
	EmojiData{ .emoji = "📚", .name = "books", .group = GRP(6), .keywords = EKW_1267, .skinToneSupport = false},
	EmojiData{ .emoji = "📓", .name = "notebook", .group = GRP(6), .keywords = EKW_1268, .skinToneSupport = false},
	EmojiData{ .emoji = "📒", .name = "ledger", .group = GRP(6), .keywords = EKW_1269, .skinToneSupport = false},
	EmojiData{ .emoji = "📃", .name = "page with curl", .group = GRP(6), .keywords = EKW_1270, .skinToneSupport = false},
	EmojiData{ .emoji = "📜", .name = "scroll", .group = GRP(6), .keywords = EKW_1271, .skinToneSupport = false},
	EmojiData{ .emoji = "📄", .name = "page facing up", .group = GRP(6), .keywords = EKW_1272, .skinToneSupport = false},
	EmojiData{ .emoji = "📰", .name = "newspaper", .group = GRP(6), .keywords = EKW_1273, .skinToneSupport = false},
	EmojiData{ .emoji = "🗞️", .name = "rolled-up newspaper", .group = GRP(6), .keywords = EKW_1274, .skinToneSupport = false},
	EmojiData{ .emoji = "📑", .name = "bookmark tabs", .group = GRP(6), .keywords = EKW_1275, .skinToneSupport = false},
	EmojiData{ .emoji = "🔖", .name = "bookmark", .group = GRP(6), .keywords = EKW_1276, .skinToneSupport = false},
	EmojiData{ .emoji = "🏷️", .name = "label", .group = GRP(6), .keywords = EKW_1277, .skinToneSupport = false},
	EmojiData{ .emoji = "💰", .name = "money bag", .group = GRP(6), .keywords = EKW_1278, .skinToneSupport = false},
	EmojiData{ .emoji = "🪙", .name = "coin", .group = GRP(6), .keywords = EKW_1279, .skinToneSupport = false},
	EmojiData{ .emoji = "💴", .name = "yen banknote", .group = GRP(6), .keywords = EKW_1280, .skinToneSupport = false},
	EmojiData{ .emoji = "💵", .name = "dollar banknote", .group = GRP(6), .keywords = EKW_1281, .skinToneSupport = false},
	EmojiData{ .emoji = "💶", .name = "euro banknote", .group = GRP(6), .keywords = EKW_1282, .skinToneSupport = false},
	EmojiData{ .emoji = "💷", .name = "pound banknote", .group = GRP(6), .keywords = EKW_1283, .skinToneSupport = false},
	EmojiData{ .emoji = "💸", .name = "money with wings", .group = GRP(6), .keywords = EKW_1284, .skinToneSupport = false},
	EmojiData{ .emoji = "💳", .name = "credit card", .group = GRP(6), .keywords = EKW_1285, .skinToneSupport = false},
	EmojiData{ .emoji = "🧾", .name = "receipt", .group = GRP(6), .keywords = EKW_1286, .skinToneSupport = false},
	EmojiData{ .emoji = "💹", .name = "chart increasing with yen", .group = GRP(6), .keywords = EKW_1287, .skinToneSupport = false},
	EmojiData{ .emoji = "✉️", .name = "envelope", .group = GRP(6), .keywords = EKW_1288, .skinToneSupport = false},
	EmojiData{ .emoji = "📧", .name = "e-mail", .group = GRP(6), .keywords = EKW_1289, .skinToneSupport = false},
	EmojiData{ .emoji = "📨", .name = "incoming envelope", .group = GRP(6), .keywords = EKW_1290, .skinToneSupport = false},
	EmojiData{ .emoji = "📩", .name = "envelope with arrow", .group = GRP(6), .keywords = EKW_1291, .skinToneSupport = false},
	EmojiData{ .emoji = "📤", .name = "outbox tray", .group = GRP(6), .keywords = EKW_1292, .skinToneSupport = false},
	EmojiData{ .emoji = "📥", .name = "inbox tray", .group = GRP(6), .keywords = EKW_1293, .skinToneSupport = false},
	EmojiData{ .emoji = "📦", .name = "package", .group = GRP(6), .keywords = EKW_1294, .skinToneSupport = false},
	EmojiData{ .emoji = "📫", .name = "closed mailbox with raised flag", .group = GRP(6), .keywords = EKW_1295, .skinToneSupport = false},
	EmojiData{ .emoji = "📪", .name = "closed mailbox with lowered flag", .group = GRP(6), .keywords = EKW_1296, .skinToneSupport = false},
	EmojiData{ .emoji = "📬", .name = "open mailbox with raised flag", .group = GRP(6), .keywords = EKW_1297, .skinToneSupport = false},
	EmojiData{ .emoji = "📭", .name = "open mailbox with lowered flag", .group = GRP(6), .keywords = EKW_1298, .skinToneSupport = false},
	EmojiData{ .emoji = "📮", .name = "postbox", .group = GRP(6), .keywords = EKW_1299, .skinToneSupport = false},
	EmojiData{ .emoji = "🗳️", .name = "ballot box with ballot", .group = GRP(6), .keywords = EKW_1300, .skinToneSupport = false},
	EmojiData{ .emoji = "✏️", .name = "pencil", .group = GRP(6), .keywords = EKW_1301, .skinToneSupport = false},
	EmojiData{ .emoji = "✒️", .name = "black nib", .group = GRP(6), .keywords = EKW_1302, .skinToneSupport = false},
	EmojiData{ .emoji = "🖋️", .name = "fountain pen", .group = GRP(6), .keywords = EKW_1303, .skinToneSupport = false},
	EmojiData{ .emoji = "🖊️", .name = "pen", .group = GRP(6), .keywords = EKW_1304, .skinToneSupport = false},
	EmojiData{ .emoji = "🖌️", .name = "paintbrush", .group = GRP(6), .keywords = EKW_1305, .skinToneSupport = false},
	EmojiData{ .emoji = "🖍️", .name = "crayon", .group = GRP(6), .keywords = EKW_1306, .skinToneSupport = false},
	EmojiData{ .emoji = "📝", .name = "memo", .group = GRP(6), .keywords = EKW_1307, .skinToneSupport = false},
	EmojiData{ .emoji = "💼", .name = "briefcase", .group = GRP(6), .keywords = EKW_1308, .skinToneSupport = false},
	EmojiData{ .emoji = "📁", .name = "file folder", .group = GRP(6), .keywords = EKW_1309, .skinToneSupport = false},
	EmojiData{ .emoji = "📂", .name = "open file folder", .group = GRP(6), .keywords = EKW_1310, .skinToneSupport = false},
	EmojiData{ .emoji = "🗂️", .name = "card index dividers", .group = GRP(6), .keywords = EKW_1311, .skinToneSupport = false},
	EmojiData{ .emoji = "📅", .name = "calendar", .group = GRP(6), .keywords = EKW_1312, .skinToneSupport = false},
	EmojiData{ .emoji = "📆", .name = "tear-off calendar", .group = GRP(6), .keywords = EKW_1313, .skinToneSupport = false},
	EmojiData{ .emoji = "🗒️", .name = "spiral notepad", .group = GRP(6), .keywords = EKW_1314, .skinToneSupport = false},
	EmojiData{ .emoji = "🗓️", .name = "spiral calendar", .group = GRP(6), .keywords = EKW_1315, .skinToneSupport = false},
	EmojiData{ .emoji = "📇", .name = "card index", .group = GRP(6), .keywords = EKW_1316, .skinToneSupport = false},
	EmojiData{ .emoji = "📈", .name = "chart increasing", .group = GRP(6), .keywords = EKW_1317, .skinToneSupport = false},
	EmojiData{ .emoji = "📉", .name = "chart decreasing", .group = GRP(6), .keywords = EKW_1318, .skinToneSupport = false},
	EmojiData{ .emoji = "📊", .name = "bar chart", .group = GRP(6), .keywords = EKW_1319, .skinToneSupport = false},
	EmojiData{ .emoji = "📋", .name = "clipboard", .group = GRP(6), .keywords = EKW_1320, .skinToneSupport = false},
	EmojiData{ .emoji = "📌", .name = "pushpin", .group = GRP(6), .keywords = EKW_1321, .skinToneSupport = false},
	EmojiData{ .emoji = "📍", .name = "round pushpin", .group = GRP(6), .keywords = EKW_1322, .skinToneSupport = false},
	EmojiData{ .emoji = "📎", .name = "paperclip", .group = GRP(6), .keywords = EKW_1323, .skinToneSupport = false},
	EmojiData{ .emoji = "🖇️", .name = "linked paperclips", .group = GRP(6), .keywords = EKW_1324, .skinToneSupport = false},
	EmojiData{ .emoji = "📏", .name = "straight ruler", .group = GRP(6), .keywords = EKW_1325, .skinToneSupport = false},
	EmojiData{ .emoji = "📐", .name = "triangular ruler", .group = GRP(6), .keywords = EKW_1326, .skinToneSupport = false},
	EmojiData{ .emoji = "✂️", .name = "scissors", .group = GRP(6), .keywords = EKW_1327, .skinToneSupport = false},
	EmojiData{ .emoji = "🗃️", .name = "card file box", .group = GRP(6), .keywords = EKW_1328, .skinToneSupport = false},
	EmojiData{ .emoji = "🗄️", .name = "file cabinet", .group = GRP(6), .keywords = EKW_1329, .skinToneSupport = false},
	EmojiData{ .emoji = "🗑️", .name = "wastebasket", .group = GRP(6), .keywords = EKW_1330, .skinToneSupport = false},
	EmojiData{ .emoji = "🔒", .name = "locked", .group = GRP(6), .keywords = EKW_1331, .skinToneSupport = false},
	EmojiData{ .emoji = "🔓", .name = "unlocked", .group = GRP(6), .keywords = EKW_1332, .skinToneSupport = false},
	EmojiData{ .emoji = "🔏", .name = "locked with pen", .group = GRP(6), .keywords = EKW_1333, .skinToneSupport = false},
	EmojiData{ .emoji = "🔐", .name = "locked with key", .group = GRP(6), .keywords = EKW_1334, .skinToneSupport = false},
	EmojiData{ .emoji = "🔑", .name = "key", .group = GRP(6), .keywords = EKW_1335, .skinToneSupport = false},
	EmojiData{ .emoji = "🗝️", .name = "old key", .group = GRP(6), .keywords = EKW_1336, .skinToneSupport = false},
	EmojiData{ .emoji = "🔨", .name = "hammer", .group = GRP(6), .keywords = EKW_1337, .skinToneSupport = false},
	EmojiData{ .emoji = "🪓", .name = "axe", .group = GRP(6), .keywords = EKW_1338, .skinToneSupport = false},
	EmojiData{ .emoji = "⛏️", .name = "pick", .group = GRP(6), .keywords = EKW_1339, .skinToneSupport = false},
	EmojiData{ .emoji = "⚒️", .name = "hammer and pick", .group = GRP(6), .keywords = EKW_1340, .skinToneSupport = false},
	EmojiData{ .emoji = "🛠️", .name = "hammer and wrench", .group = GRP(6), .keywords = EKW_1341, .skinToneSupport = false},
	EmojiData{ .emoji = "🗡️", .name = "dagger", .group = GRP(6), .keywords = EKW_1342, .skinToneSupport = false},
	EmojiData{ .emoji = "⚔️", .name = "crossed swords", .group = GRP(6), .keywords = EKW_1343, .skinToneSupport = false},
	EmojiData{ .emoji = "💣", .name = "bomb", .group = GRP(6), .keywords = EKW_1344, .skinToneSupport = false},
	EmojiData{ .emoji = "🪃", .name = "boomerang", .group = GRP(6), .keywords = EKW_1345, .skinToneSupport = false},
	EmojiData{ .emoji = "🏹", .name = "bow and arrow", .group = GRP(6), .keywords = EKW_1346, .skinToneSupport = false},
	EmojiData{ .emoji = "🛡️", .name = "shield", .group = GRP(6), .keywords = EKW_1347, .skinToneSupport = false},
	EmojiData{ .emoji = "🪚", .name = "carpentry saw", .group = GRP(6), .keywords = EKW_1348, .skinToneSupport = false},
	EmojiData{ .emoji = "🔧", .name = "wrench", .group = GRP(6), .keywords = EKW_1349, .skinToneSupport = false},
	EmojiData{ .emoji = "🪛", .name = "screwdriver", .group = GRP(6), .keywords = EKW_1350, .skinToneSupport = false},
	EmojiData{ .emoji = "🔩", .name = "nut and bolt", .group = GRP(6), .keywords = EKW_1351, .skinToneSupport = false},
	EmojiData{ .emoji = "⚙️", .name = "gear", .group = GRP(6), .keywords = EKW_1352, .skinToneSupport = false},
	EmojiData{ .emoji = "🗜️", .name = "clamp", .group = GRP(6), .keywords = EKW_1353, .skinToneSupport = false},
	EmojiData{ .emoji = "⚖️", .name = "balance scale", .group = GRP(6), .keywords = EKW_1354, .skinToneSupport = false},
	EmojiData{ .emoji = "🦯", .name = "white cane", .group = GRP(6), .keywords = EKW_1355, .skinToneSupport = false},
	EmojiData{ .emoji = "🔗", .name = "link", .group = GRP(6), .keywords = EKW_1356, .skinToneSupport = false},
	EmojiData{ .emoji = "⛓️‍💥", .name = "broken chain", .group = GRP(6), .keywords = EKW_1357, .skinToneSupport = false},
	EmojiData{ .emoji = "⛓️", .name = "chains", .group = GRP(6), .keywords = EKW_1358, .skinToneSupport = false},
	EmojiData{ .emoji = "🪝", .name = "hook", .group = GRP(6), .keywords = EKW_1359, .skinToneSupport = false},
	EmojiData{ .emoji = "🧰", .name = "toolbox", .group = GRP(6), .keywords = EKW_1360, .skinToneSupport = false},
	EmojiData{ .emoji = "🧲", .name = "magnet", .group = GRP(6), .keywords = EKW_1361, .skinToneSupport = false},
	EmojiData{ .emoji = "🪜", .name = "ladder", .group = GRP(6), .keywords = EKW_1362, .skinToneSupport = false},
	EmojiData{ .emoji = "🪏", .name = "shovel", .group = GRP(6), .keywords = EKW_1363, .skinToneSupport = false},
	EmojiData{ .emoji = "⚗️", .name = "alembic", .group = GRP(6), .keywords = EKW_1364, .skinToneSupport = false},
	EmojiData{ .emoji = "🧪", .name = "test tube", .group = GRP(6), .keywords = EKW_1365, .skinToneSupport = false},
	EmojiData{ .emoji = "🧫", .name = "petri dish", .group = GRP(6), .keywords = EKW_1366, .skinToneSupport = false},
	EmojiData{ .emoji = "🧬", .name = "dna", .group = GRP(6), .keywords = EKW_1367, .skinToneSupport = false},
	EmojiData{ .emoji = "🔬", .name = "microscope", .group = GRP(6), .keywords = EKW_1368, .skinToneSupport = false},
	EmojiData{ .emoji = "🔭", .name = "telescope", .group = GRP(6), .keywords = EKW_1369, .skinToneSupport = false},
	EmojiData{ .emoji = "📡", .name = "satellite antenna", .group = GRP(6), .keywords = EKW_1370, .skinToneSupport = false},
	EmojiData{ .emoji = "💉", .name = "syringe", .group = GRP(6), .keywords = EKW_1371, .skinToneSupport = false},
	EmojiData{ .emoji = "🩸", .name = "drop of blood", .group = GRP(6), .keywords = EKW_1372, .skinToneSupport = false},
	EmojiData{ .emoji = "💊", .name = "pill", .group = GRP(6), .keywords = EKW_1373, .skinToneSupport = false},
	EmojiData{ .emoji = "🩹", .name = "adhesive bandage", .group = GRP(6), .keywords = EKW_1374, .skinToneSupport = false},
	EmojiData{ .emoji = "🩼", .name = "crutch", .group = GRP(6), .keywords = EKW_1375, .skinToneSupport = false},
	EmojiData{ .emoji = "🩺", .name = "stethoscope", .group = GRP(6), .keywords = EKW_1376, .skinToneSupport = false},
	EmojiData{ .emoji = "🩻", .name = "x-ray", .group = GRP(6), .keywords = EKW_1377, .skinToneSupport = false},
	EmojiData{ .emoji = "🚪", .name = "door", .group = GRP(6), .keywords = EKW_1378, .skinToneSupport = false},
	EmojiData{ .emoji = "🛗", .name = "elevator", .group = GRP(6), .keywords = EKW_1379, .skinToneSupport = false},
	EmojiData{ .emoji = "🪞", .name = "mirror", .group = GRP(6), .keywords = EKW_1380, .skinToneSupport = false},
	EmojiData{ .emoji = "🪟", .name = "window", .group = GRP(6), .keywords = EKW_1381, .skinToneSupport = false},
	EmojiData{ .emoji = "🛏️", .name = "bed", .group = GRP(6), .keywords = EKW_1382, .skinToneSupport = false},
	EmojiData{ .emoji = "🛋️", .name = "couch and lamp", .group = GRP(6), .keywords = EKW_1383, .skinToneSupport = false},
	EmojiData{ .emoji = "🪑", .name = "chair", .group = GRP(6), .keywords = EKW_1384, .skinToneSupport = false},
	EmojiData{ .emoji = "🚽", .name = "toilet", .group = GRP(6), .keywords = EKW_1385, .skinToneSupport = false},
	EmojiData{ .emoji = "🪠", .name = "plunger", .group = GRP(6), .keywords = EKW_1386, .skinToneSupport = false},
	EmojiData{ .emoji = "🚿", .name = "shower", .group = GRP(6), .keywords = EKW_1387, .skinToneSupport = false},
	EmojiData{ .emoji = "🛁", .name = "bathtub", .group = GRP(6), .keywords = EKW_1388, .skinToneSupport = false},
	EmojiData{ .emoji = "🪤", .name = "mouse trap", .group = GRP(6), .keywords = EKW_1389, .skinToneSupport = false},
	EmojiData{ .emoji = "🪒", .name = "razor", .group = GRP(6), .keywords = EKW_1390, .skinToneSupport = false},
	EmojiData{ .emoji = "🧴", .name = "lotion bottle", .group = GRP(6), .keywords = EKW_1391, .skinToneSupport = false},
	EmojiData{ .emoji = "🧷", .name = "safety pin", .group = GRP(6), .keywords = EKW_1392, .skinToneSupport = false},
	EmojiData{ .emoji = "🧹", .name = "broom", .group = GRP(6), .keywords = EKW_1393, .skinToneSupport = false},
	EmojiData{ .emoji = "🧺", .name = "basket", .group = GRP(6), .keywords = EKW_1394, .skinToneSupport = false},
	EmojiData{ .emoji = "🧻", .name = "roll of paper", .group = GRP(6), .keywords = EKW_1395, .skinToneSupport = false},
	EmojiData{ .emoji = "🪣", .name = "bucket", .group = GRP(6), .keywords = EKW_1396, .skinToneSupport = false},
	EmojiData{ .emoji = "🧼", .name = "soap", .group = GRP(6), .keywords = EKW_1397, .skinToneSupport = false},
	EmojiData{ .emoji = "🫧", .name = "bubbles", .group = GRP(6), .keywords = EKW_1398, .skinToneSupport = false},
	EmojiData{ .emoji = "🪥", .name = "toothbrush", .group = GRP(6), .keywords = EKW_1399, .skinToneSupport = false},
	EmojiData{ .emoji = "🧽", .name = "sponge", .group = GRP(6), .keywords = EKW_1400, .skinToneSupport = false},
	EmojiData{ .emoji = "🧯", .name = "fire extinguisher", .group = GRP(6), .keywords = EKW_1401, .skinToneSupport = false},
	EmojiData{ .emoji = "🛒", .name = "shopping cart", .group = GRP(6), .keywords = EKW_1402, .skinToneSupport = false},
	EmojiData{ .emoji = "🚬", .name = "cigarette", .group = GRP(6), .keywords = EKW_1403, .skinToneSupport = false},
	EmojiData{ .emoji = "⚰️", .name = "coffin", .group = GRP(6), .keywords = EKW_1404, .skinToneSupport = false},
	EmojiData{ .emoji = "🪦", .name = "headstone", .group = GRP(6), .keywords = EKW_1405, .skinToneSupport = false},
	EmojiData{ .emoji = "⚱️", .name = "funeral urn", .group = GRP(6), .keywords = EKW_1406, .skinToneSupport = false},
	EmojiData{ .emoji = "🧿", .name = "nazar amulet", .group = GRP(6), .keywords = EKW_1407, .skinToneSupport = false},
	EmojiData{ .emoji = "🪬", .name = "hamsa", .group = GRP(6), .keywords = EKW_1408, .skinToneSupport = false},
	EmojiData{ .emoji = "🗿", .name = "moai", .group = GRP(6), .keywords = EKW_1409, .skinToneSupport = false},
	EmojiData{ .emoji = "🪧", .name = "placard", .group = GRP(6), .keywords = EKW_1410, .skinToneSupport = false},
	EmojiData{ .emoji = "🪪", .name = "identification card", .group = GRP(6), .keywords = EKW_1411, .skinToneSupport = false},
	EmojiData{ .emoji = "🏧", .name = "ATM sign", .group = GRP(7), .keywords = EKW_1412, .skinToneSupport = false},
	EmojiData{ .emoji = "🚮", .name = "litter in bin sign", .group = GRP(7), .keywords = EKW_1413, .skinToneSupport = false},
	EmojiData{ .emoji = "🚰", .name = "potable water", .group = GRP(7), .keywords = EKW_1414, .skinToneSupport = false},
	EmojiData{ .emoji = "♿", .name = "wheelchair symbol", .group = GRP(7), .keywords = EKW_1415, .skinToneSupport = false},
	EmojiData{ .emoji = "🚹", .name = "men’s room", .group = GRP(7), .keywords = EKW_1416, .skinToneSupport = false},
	EmojiData{ .emoji = "🚺", .name = "women’s room", .group = GRP(7), .keywords = EKW_1417, .skinToneSupport = false},
	EmojiData{ .emoji = "🚻", .name = "restroom", .group = GRP(7), .keywords = EKW_1418, .skinToneSupport = false},
	EmojiData{ .emoji = "🚼", .name = "baby symbol", .group = GRP(7), .keywords = EKW_1419, .skinToneSupport = false},
	EmojiData{ .emoji = "🚾", .name = "water closet", .group = GRP(7), .keywords = EKW_1420, .skinToneSupport = false},
	EmojiData{ .emoji = "🛂", .name = "passport control", .group = GRP(7), .keywords = EKW_1421, .skinToneSupport = false},
	EmojiData{ .emoji = "🛃", .name = "customs", .group = GRP(7), .keywords = EKW_1422, .skinToneSupport = false},
	EmojiData{ .emoji = "🛄", .name = "baggage claim", .group = GRP(7), .keywords = EKW_1423, .skinToneSupport = false},
	EmojiData{ .emoji = "🛅", .name = "left luggage", .group = GRP(7), .keywords = EKW_1424, .skinToneSupport = false},
	EmojiData{ .emoji = "⚠️", .name = "warning", .group = GRP(7), .keywords = EKW_1425, .skinToneSupport = false},
	EmojiData{ .emoji = "🚸", .name = "children crossing", .group = GRP(7), .keywords = EKW_1426, .skinToneSupport = false},
	EmojiData{ .emoji = "⛔", .name = "no entry", .group = GRP(7), .keywords = EKW_1427, .skinToneSupport = false},
	EmojiData{ .emoji = "🚫", .name = "prohibited", .group = GRP(7), .keywords = EKW_1428, .skinToneSupport = false},
	EmojiData{ .emoji = "🚳", .name = "no bicycles", .group = GRP(7), .keywords = EKW_1429, .skinToneSupport = false},
	EmojiData{ .emoji = "🚭", .name = "no smoking", .group = GRP(7), .keywords = EKW_1430, .skinToneSupport = false},
	EmojiData{ .emoji = "🚯", .name = "no littering", .group = GRP(7), .keywords = EKW_1431, .skinToneSupport = false},
	EmojiData{ .emoji = "🚱", .name = "non-potable water", .group = GRP(7), .keywords = EKW_1432, .skinToneSupport = false},
	EmojiData{ .emoji = "🚷", .name = "no pedestrians", .group = GRP(7), .keywords = EKW_1433, .skinToneSupport = false},
	EmojiData{ .emoji = "📵", .name = "no mobile phones", .group = GRP(7), .keywords = EKW_1434, .skinToneSupport = false},
	EmojiData{ .emoji = "🔞", .name = "no one under eighteen", .group = GRP(7), .keywords = EKW_1435, .skinToneSupport = false},
	EmojiData{ .emoji = "☢️", .name = "radioactive", .group = GRP(7), .keywords = EKW_1436, .skinToneSupport = false},
	EmojiData{ .emoji = "☣️", .name = "biohazard", .group = GRP(7), .keywords = EKW_1437, .skinToneSupport = false},
	EmojiData{ .emoji = "⬆️", .name = "up arrow", .group = GRP(7), .keywords = EKW_1438, .skinToneSupport = false},
	EmojiData{ .emoji = "↗️", .name = "up-right arrow", .group = GRP(7), .keywords = EKW_1439, .skinToneSupport = false},
	EmojiData{ .emoji = "➡️", .name = "right arrow", .group = GRP(7), .keywords = EKW_1440, .skinToneSupport = false},
	EmojiData{ .emoji = "↘️", .name = "down-right arrow", .group = GRP(7), .keywords = EKW_1441, .skinToneSupport = false},
	EmojiData{ .emoji = "⬇️", .name = "down arrow", .group = GRP(7), .keywords = EKW_1442, .skinToneSupport = false},
	EmojiData{ .emoji = "↙️", .name = "down-left arrow", .group = GRP(7), .keywords = EKW_1443, .skinToneSupport = false},
	EmojiData{ .emoji = "⬅️", .name = "left arrow", .group = GRP(7), .keywords = EKW_1444, .skinToneSupport = false},
	EmojiData{ .emoji = "↖️", .name = "up-left arrow", .group = GRP(7), .keywords = EKW_1445, .skinToneSupport = false},
	EmojiData{ .emoji = "↕️", .name = "up-down arrow", .group = GRP(7), .keywords = EKW_1446, .skinToneSupport = false},
	EmojiData{ .emoji = "↔️", .name = "left-right arrow", .group = GRP(7), .keywords = EKW_1447, .skinToneSupport = false},
	EmojiData{ .emoji = "↩️", .name = "right arrow curving left", .group = GRP(7), .keywords = EKW_1448, .skinToneSupport = false},
	EmojiData{ .emoji = "↪️", .name = "left arrow curving right", .group = GRP(7), .keywords = EKW_1449, .skinToneSupport = false},
	EmojiData{ .emoji = "⤴️", .name = "right arrow curving up", .group = GRP(7), .keywords = EKW_1450, .skinToneSupport = false},
	EmojiData{ .emoji = "⤵️", .name = "right arrow curving down", .group = GRP(7), .keywords = EKW_1451, .skinToneSupport = false},
	EmojiData{ .emoji = "🔃", .name = "clockwise vertical arrows", .group = GRP(7), .keywords = EKW_1452, .skinToneSupport = false},
	EmojiData{ .emoji = "🔄", .name = "counterclockwise arrows button", .group = GRP(7), .keywords = EKW_1453, .skinToneSupport = false},
	EmojiData{ .emoji = "🔙", .name = "BACK arrow", .group = GRP(7), .keywords = EKW_1454, .skinToneSupport = false},
	EmojiData{ .emoji = "🔚", .name = "END arrow", .group = GRP(7), .keywords = EKW_1455, .skinToneSupport = false},
	EmojiData{ .emoji = "🔛", .name = "ON! arrow", .group = GRP(7), .keywords = EKW_1456, .skinToneSupport = false},
	EmojiData{ .emoji = "🔜", .name = "SOON arrow", .group = GRP(7), .keywords = EKW_1457, .skinToneSupport = false},
	EmojiData{ .emoji = "🔝", .name = "TOP arrow", .group = GRP(7), .keywords = EKW_1458, .skinToneSupport = false},
	EmojiData{ .emoji = "🛐", .name = "place of worship", .group = GRP(7), .keywords = EKW_1459, .skinToneSupport = false},
	EmojiData{ .emoji = "⚛️", .name = "atom symbol", .group = GRP(7), .keywords = EKW_1460, .skinToneSupport = false},
	EmojiData{ .emoji = "🕉️", .name = "om", .group = GRP(7), .keywords = EKW_1461, .skinToneSupport = false},
	EmojiData{ .emoji = "✡️", .name = "star of David", .group = GRP(7), .keywords = EKW_1462, .skinToneSupport = false},
	EmojiData{ .emoji = "☸️", .name = "wheel of dharma", .group = GRP(7), .keywords = EKW_1463, .skinToneSupport = false},
	EmojiData{ .emoji = "☯️", .name = "yin yang", .group = GRP(7), .keywords = EKW_1464, .skinToneSupport = false},
	EmojiData{ .emoji = "✝️", .name = "latin cross", .group = GRP(7), .keywords = EKW_1465, .skinToneSupport = false},
	EmojiData{ .emoji = "☦️", .name = "orthodox cross", .group = GRP(7), .keywords = EKW_1466, .skinToneSupport = false},
	EmojiData{ .emoji = "☪️", .name = "star and crescent", .group = GRP(7), .keywords = EKW_1467, .skinToneSupport = false},
	EmojiData{ .emoji = "☮️", .name = "peace symbol", .group = GRP(7), .keywords = EKW_1468, .skinToneSupport = false},
	EmojiData{ .emoji = "🕎", .name = "menorah", .group = GRP(7), .keywords = EKW_1469, .skinToneSupport = false},
	EmojiData{ .emoji = "🔯", .name = "dotted six-pointed star", .group = GRP(7), .keywords = EKW_1470, .skinToneSupport = false},
	EmojiData{ .emoji = "🪯", .name = "khanda", .group = GRP(7), .keywords = EKW_1471, .skinToneSupport = false},
	EmojiData{ .emoji = "♈", .name = "Aries", .group = GRP(7), .keywords = EKW_1472, .skinToneSupport = false},
	EmojiData{ .emoji = "♉", .name = "Taurus", .group = GRP(7), .keywords = EKW_1473, .skinToneSupport = false},
	EmojiData{ .emoji = "♊", .name = "Gemini", .group = GRP(7), .keywords = EKW_1474, .skinToneSupport = false},
	EmojiData{ .emoji = "♋", .name = "Cancer", .group = GRP(7), .keywords = EKW_1475, .skinToneSupport = false},
	EmojiData{ .emoji = "♌", .name = "Leo", .group = GRP(7), .keywords = EKW_1476, .skinToneSupport = false},
	EmojiData{ .emoji = "♍", .name = "Virgo", .group = GRP(7), .keywords = EKW_1477, .skinToneSupport = false},
	EmojiData{ .emoji = "♎", .name = "Libra", .group = GRP(7), .keywords = EKW_1478, .skinToneSupport = false},
	EmojiData{ .emoji = "♏", .name = "Scorpio", .group = GRP(7), .keywords = EKW_1479, .skinToneSupport = false},
	EmojiData{ .emoji = "♐", .name = "Sagittarius", .group = GRP(7), .keywords = EKW_1480, .skinToneSupport = false},
	EmojiData{ .emoji = "♑", .name = "Capricorn", .group = GRP(7), .keywords = EKW_1481, .skinToneSupport = false},
	EmojiData{ .emoji = "♒", .name = "Aquarius", .group = GRP(7), .keywords = EKW_1482, .skinToneSupport = false},
	EmojiData{ .emoji = "♓", .name = "Pisces", .group = GRP(7), .keywords = EKW_1483, .skinToneSupport = false},
	EmojiData{ .emoji = "⛎", .name = "Ophiuchus", .group = GRP(7), .keywords = EKW_1484, .skinToneSupport = false},
	EmojiData{ .emoji = "🔀", .name = "shuffle tracks button", .group = GRP(7), .keywords = EKW_1485, .skinToneSupport = false},
	EmojiData{ .emoji = "🔁", .name = "repeat button", .group = GRP(7), .keywords = EKW_1486, .skinToneSupport = false},
	EmojiData{ .emoji = "🔂", .name = "repeat single button", .group = GRP(7), .keywords = EKW_1487, .skinToneSupport = false},
	EmojiData{ .emoji = "▶️", .name = "play button", .group = GRP(7), .keywords = EKW_1488, .skinToneSupport = false},
	EmojiData{ .emoji = "⏩", .name = "fast-forward button", .group = GRP(7), .keywords = EKW_1489, .skinToneSupport = false},
	EmojiData{ .emoji = "⏭️", .name = "next track button", .group = GRP(7), .keywords = EKW_1490, .skinToneSupport = false},
	EmojiData{ .emoji = "⏯️", .name = "play or pause button", .group = GRP(7), .keywords = EKW_1491, .skinToneSupport = false},
	EmojiData{ .emoji = "◀️", .name = "reverse button", .group = GRP(7), .keywords = EKW_1492, .skinToneSupport = false},
	EmojiData{ .emoji = "⏪", .name = "fast reverse button", .group = GRP(7), .keywords = EKW_1493, .skinToneSupport = false},
	EmojiData{ .emoji = "⏮️", .name = "last track button", .group = GRP(7), .keywords = EKW_1494, .skinToneSupport = false},
	EmojiData{ .emoji = "🔼", .name = "upwards button", .group = GRP(7), .keywords = EKW_1495, .skinToneSupport = false},
	EmojiData{ .emoji = "⏫", .name = "fast up button", .group = GRP(7), .keywords = EKW_1496, .skinToneSupport = false},
	EmojiData{ .emoji = "🔽", .name = "downwards button", .group = GRP(7), .keywords = EKW_1497, .skinToneSupport = false},
	EmojiData{ .emoji = "⏬", .name = "fast down button", .group = GRP(7), .keywords = EKW_1498, .skinToneSupport = false},
	EmojiData{ .emoji = "⏸️", .name = "pause button", .group = GRP(7), .keywords = EKW_1499, .skinToneSupport = false},
	EmojiData{ .emoji = "⏹️", .name = "stop button", .group = GRP(7), .keywords = EKW_1500, .skinToneSupport = false},
	EmojiData{ .emoji = "⏺️", .name = "record button", .group = GRP(7), .keywords = EKW_1501, .skinToneSupport = false},
	EmojiData{ .emoji = "⏏️", .name = "eject button", .group = GRP(7), .keywords = EKW_1502, .skinToneSupport = false},
	EmojiData{ .emoji = "🎦", .name = "cinema", .group = GRP(7), .keywords = EKW_1503, .skinToneSupport = false},
	EmojiData{ .emoji = "🔅", .name = "dim button", .group = GRP(7), .keywords = EKW_1504, .skinToneSupport = false},
	EmojiData{ .emoji = "🔆", .name = "bright button", .group = GRP(7), .keywords = EKW_1505, .skinToneSupport = false},
	EmojiData{ .emoji = "📶", .name = "antenna bars", .group = GRP(7), .keywords = EKW_1506, .skinToneSupport = false},
	EmojiData{ .emoji = "🛜", .name = "wireless", .group = GRP(7), .keywords = EKW_1507, .skinToneSupport = false},
	EmojiData{ .emoji = "📳", .name = "vibration mode", .group = GRP(7), .keywords = EKW_1508, .skinToneSupport = false},
	EmojiData{ .emoji = "📴", .name = "mobile phone off", .group = GRP(7), .keywords = EKW_1509, .skinToneSupport = false},
	EmojiData{ .emoji = "♀️", .name = "female sign", .group = GRP(7), .keywords = EKW_1510, .skinToneSupport = false},
	EmojiData{ .emoji = "♂️", .name = "male sign", .group = GRP(7), .keywords = EKW_1511, .skinToneSupport = false},
	EmojiData{ .emoji = "⚧️", .name = "transgender symbol", .group = GRP(7), .keywords = EKW_1512, .skinToneSupport = false},
	EmojiData{ .emoji = "✖️", .name = "multiply", .group = GRP(7), .keywords = EKW_1513, .skinToneSupport = false},
	EmojiData{ .emoji = "➕", .name = "plus", .group = GRP(7), .keywords = EKW_1514, .skinToneSupport = false},
	EmojiData{ .emoji = "➖", .name = "minus", .group = GRP(7), .keywords = EKW_1515, .skinToneSupport = false},
	EmojiData{ .emoji = "➗", .name = "divide", .group = GRP(7), .keywords = EKW_1516, .skinToneSupport = false},
	EmojiData{ .emoji = "🟰", .name = "heavy equals sign", .group = GRP(7), .keywords = EKW_1517, .skinToneSupport = false},
	EmojiData{ .emoji = "♾️", .name = "infinity", .group = GRP(7), .keywords = EKW_1518, .skinToneSupport = false},
	EmojiData{ .emoji = "‼️", .name = "double exclamation mark", .group = GRP(7), .keywords = EKW_1519, .skinToneSupport = false},
	EmojiData{ .emoji = "⁉️", .name = "exclamation question mark", .group = GRP(7), .keywords = EKW_1520, .skinToneSupport = false},
	EmojiData{ .emoji = "❓", .name = "red question mark", .group = GRP(7), .keywords = EKW_1521, .skinToneSupport = false},
	EmojiData{ .emoji = "❔", .name = "white question mark", .group = GRP(7), .keywords = EKW_1522, .skinToneSupport = false},
	EmojiData{ .emoji = "❕", .name = "white exclamation mark", .group = GRP(7), .keywords = EKW_1523, .skinToneSupport = false},
	EmojiData{ .emoji = "❗", .name = "red exclamation mark", .group = GRP(7), .keywords = EKW_1524, .skinToneSupport = false},
	EmojiData{ .emoji = "〰️", .name = "wavy dash", .group = GRP(7), .keywords = EKW_1525, .skinToneSupport = false},
	EmojiData{ .emoji = "💱", .name = "currency exchange", .group = GRP(7), .keywords = EKW_1526, .skinToneSupport = false},
	EmojiData{ .emoji = "💲", .name = "heavy dollar sign", .group = GRP(7), .keywords = EKW_1527, .skinToneSupport = false},
	EmojiData{ .emoji = "⚕️", .name = "medical symbol", .group = GRP(7), .keywords = EKW_1528, .skinToneSupport = false},
	EmojiData{ .emoji = "♻️", .name = "recycling symbol", .group = GRP(7), .keywords = EKW_1529, .skinToneSupport = false},
	EmojiData{ .emoji = "⚜️", .name = "fleur-de-lis", .group = GRP(7), .keywords = EKW_1530, .skinToneSupport = false},
	EmojiData{ .emoji = "🔱", .name = "trident emblem", .group = GRP(7), .keywords = EKW_1531, .skinToneSupport = false},
	EmojiData{ .emoji = "📛", .name = "name badge", .group = GRP(7), .keywords = EKW_1532, .skinToneSupport = false},
	EmojiData{ .emoji = "🔰", .name = "Japanese symbol for beginner", .group = GRP(7), .keywords = EKW_1533, .skinToneSupport = false},
	EmojiData{ .emoji = "⭕", .name = "hollow red circle", .group = GRP(7), .keywords = EKW_1534, .skinToneSupport = false},
	EmojiData{ .emoji = "✅", .name = "check mark button", .group = GRP(7), .keywords = EKW_1535, .skinToneSupport = false},
	EmojiData{ .emoji = "☑️", .name = "check box with check", .group = GRP(7), .keywords = EKW_1536, .skinToneSupport = false},
	EmojiData{ .emoji = "✔️", .name = "check mark", .group = GRP(7), .keywords = EKW_1537, .skinToneSupport = false},
	EmojiData{ .emoji = "❌", .name = "cross mark", .group = GRP(7), .keywords = EKW_1538, .skinToneSupport = false},
	EmojiData{ .emoji = "❎", .name = "cross mark button", .group = GRP(7), .keywords = EKW_1539, .skinToneSupport = false},
	EmojiData{ .emoji = "➰", .name = "curly loop", .group = GRP(7), .keywords = EKW_1540, .skinToneSupport = false},
	EmojiData{ .emoji = "➿", .name = "double curly loop", .group = GRP(7), .keywords = EKW_1541, .skinToneSupport = false},
	EmojiData{ .emoji = "〽️", .name = "part alternation mark", .group = GRP(7), .keywords = EKW_1542, .skinToneSupport = false},
	EmojiData{ .emoji = "✳️", .name = "eight-spoked asterisk", .group = GRP(7), .keywords = EKW_1543, .skinToneSupport = false},
	EmojiData{ .emoji = "✴️", .name = "eight-pointed star", .group = GRP(7), .keywords = EKW_1544, .skinToneSupport = false},
	EmojiData{ .emoji = "❇️", .name = "sparkle", .group = GRP(7), .keywords = EKW_1545, .skinToneSupport = false},
	EmojiData{ .emoji = "©️", .name = "copyright", .group = GRP(7), .keywords = EKW_1546, .skinToneSupport = false},
	EmojiData{ .emoji = "®️", .name = "registered", .group = GRP(7), .keywords = EKW_1547, .skinToneSupport = false},
	EmojiData{ .emoji = "™️", .name = "trade mark", .group = GRP(7), .keywords = EKW_1548, .skinToneSupport = false},
	EmojiData{ .emoji = "🫟", .name = "splatter", .group = GRP(7), .keywords = EKW_1549, .skinToneSupport = false},
	EmojiData{ .emoji = "#️⃣", .name = "keycap #", .group = GRP(7), .keywords = EKW_1550, .skinToneSupport = false},
	EmojiData{ .emoji = "*️⃣", .name = "keycap *", .group = GRP(7), .keywords = EKW_1551, .skinToneSupport = false},
	EmojiData{ .emoji = "0️⃣", .name = "keycap 0", .group = GRP(7), .keywords = EKW_1552, .skinToneSupport = false},
	EmojiData{ .emoji = "1️⃣", .name = "keycap 1", .group = GRP(7), .keywords = EKW_1553, .skinToneSupport = false},
	EmojiData{ .emoji = "2️⃣", .name = "keycap 2", .group = GRP(7), .keywords = EKW_1554, .skinToneSupport = false},
	EmojiData{ .emoji = "3️⃣", .name = "keycap 3", .group = GRP(7), .keywords = EKW_1555, .skinToneSupport = false},
	EmojiData{ .emoji = "4️⃣", .name = "keycap 4", .group = GRP(7), .keywords = EKW_1556, .skinToneSupport = false},
	EmojiData{ .emoji = "5️⃣", .name = "keycap 5", .group = GRP(7), .keywords = EKW_1557, .skinToneSupport = false},
	EmojiData{ .emoji = "6️⃣", .name = "keycap 6", .group = GRP(7), .keywords = EKW_1558, .skinToneSupport = false},
	EmojiData{ .emoji = "7️⃣", .name = "keycap 7", .group = GRP(7), .keywords = EKW_1559, .skinToneSupport = false},
	EmojiData{ .emoji = "8️⃣", .name = "keycap 8", .group = GRP(7), .keywords = EKW_1560, .skinToneSupport = false},
	EmojiData{ .emoji = "9️⃣", .name = "keycap 9", .group = GRP(7), .keywords = EKW_1561, .skinToneSupport = false},
	EmojiData{ .emoji = "🔟", .name = "keycap 10", .group = GRP(7), .keywords = EKW_1562, .skinToneSupport = false},
	EmojiData{ .emoji = "🔠", .name = "input latin uppercase", .group = GRP(7), .keywords = EKW_1563, .skinToneSupport = false},
	EmojiData{ .emoji = "🔡", .name = "input latin lowercase", .group = GRP(7), .keywords = EKW_1564, .skinToneSupport = false},
	EmojiData{ .emoji = "🔢", .name = "input numbers", .group = GRP(7), .keywords = EKW_1565, .skinToneSupport = false},
	EmojiData{ .emoji = "🔣", .name = "input symbols", .group = GRP(7), .keywords = EKW_1566, .skinToneSupport = false},
	EmojiData{ .emoji = "🔤", .name = "input latin letters", .group = GRP(7), .keywords = EKW_1567, .skinToneSupport = false},
	EmojiData{ .emoji = "🅰️", .name = "A button (blood type)", .group = GRP(7), .keywords = EKW_1568, .skinToneSupport = false},
	EmojiData{ .emoji = "🆎", .name = "AB button (blood type)", .group = GRP(7), .keywords = EKW_1569, .skinToneSupport = false},
	EmojiData{ .emoji = "🅱️", .name = "B button (blood type)", .group = GRP(7), .keywords = EKW_1570, .skinToneSupport = false},
	EmojiData{ .emoji = "🆑", .name = "CL button", .group = GRP(7), .keywords = EKW_1571, .skinToneSupport = false},
	EmojiData{ .emoji = "🆒", .name = "COOL button", .group = GRP(7), .keywords = EKW_1572, .skinToneSupport = false},
	EmojiData{ .emoji = "🆓", .name = "FREE button", .group = GRP(7), .keywords = EKW_1573, .skinToneSupport = false},
	EmojiData{ .emoji = "ℹ️", .name = "information", .group = GRP(7), .keywords = EKW_1574, .skinToneSupport = false},
	EmojiData{ .emoji = "🆔", .name = "ID button", .group = GRP(7), .keywords = EKW_1575, .skinToneSupport = false},
	EmojiData{ .emoji = "Ⓜ️", .name = "circled M", .group = GRP(7), .keywords = EKW_1576, .skinToneSupport = false},
	EmojiData{ .emoji = "🆕", .name = "NEW button", .group = GRP(7), .keywords = EKW_1577, .skinToneSupport = false},
	EmojiData{ .emoji = "🆖", .name = "NG button", .group = GRP(7), .keywords = EKW_1578, .skinToneSupport = false},
	EmojiData{ .emoji = "🅾️", .name = "O button (blood type)", .group = GRP(7), .keywords = EKW_1579, .skinToneSupport = false},
	EmojiData{ .emoji = "🆗", .name = "OK button", .group = GRP(7), .keywords = EKW_1580, .skinToneSupport = false},
	EmojiData{ .emoji = "🅿️", .name = "P button", .group = GRP(7), .keywords = EKW_1581, .skinToneSupport = false},
	EmojiData{ .emoji = "🆘", .name = "SOS button", .group = GRP(7), .keywords = EKW_1582, .skinToneSupport = false},
	EmojiData{ .emoji = "🆙", .name = "UP! button", .group = GRP(7), .keywords = EKW_1583, .skinToneSupport = false},
	EmojiData{ .emoji = "🆚", .name = "VS button", .group = GRP(7), .keywords = EKW_1584, .skinToneSupport = false},
	EmojiData{ .emoji = "🈁", .name = "Japanese “here” button", .group = GRP(7), .keywords = EKW_1585, .skinToneSupport = false},
	EmojiData{ .emoji = "🈂️", .name = "Japanese “service charge” button", .group = GRP(7), .keywords = EKW_1586, .skinToneSupport = false},
	EmojiData{ .emoji = "🈷️", .name = "Japanese “monthly amount” button", .group = GRP(7), .keywords = EKW_1587, .skinToneSupport = false},
	EmojiData{ .emoji = "🈶", .name = "Japanese “not free of charge” button", .group = GRP(7), .keywords = EKW_1588, .skinToneSupport = false},
	EmojiData{ .emoji = "🈯", .name = "Japanese “reserved” button", .group = GRP(7), .keywords = EKW_1589, .skinToneSupport = false},
	EmojiData{ .emoji = "🉐", .name = "Japanese “bargain” button", .group = GRP(7), .keywords = EKW_1590, .skinToneSupport = false},
	EmojiData{ .emoji = "🈹", .name = "Japanese “discount” button", .group = GRP(7), .keywords = EKW_1591, .skinToneSupport = false},
	EmojiData{ .emoji = "🈚", .name = "Japanese “free of charge” button", .group = GRP(7), .keywords = EKW_1592, .skinToneSupport = false},
	EmojiData{ .emoji = "🈲", .name = "Japanese “prohibited” button", .group = GRP(7), .keywords = EKW_1593, .skinToneSupport = false},
	EmojiData{ .emoji = "🉑", .name = "Japanese “acceptable” button", .group = GRP(7), .keywords = EKW_1594, .skinToneSupport = false},
	EmojiData{ .emoji = "🈸", .name = "Japanese “application” button", .group = GRP(7), .keywords = EKW_1595, .skinToneSupport = false},
	EmojiData{ .emoji = "🈴", .name = "Japanese “passing grade” button", .group = GRP(7), .keywords = EKW_1596, .skinToneSupport = false},
	EmojiData{ .emoji = "🈳", .name = "Japanese “vacancy” button", .group = GRP(7), .keywords = EKW_1597, .skinToneSupport = false},
	EmojiData{ .emoji = "㊗️", .name = "Japanese “congratulations” button", .group = GRP(7), .keywords = EKW_1598, .skinToneSupport = false},
	EmojiData{ .emoji = "㊙️", .name = "Japanese “secret” button", .group = GRP(7), .keywords = EKW_1599, .skinToneSupport = false},
	EmojiData{ .emoji = "🈺", .name = "Japanese “open for business” button", .group = GRP(7), .keywords = EKW_1600, .skinToneSupport = false},
	EmojiData{ .emoji = "🈵", .name = "Japanese “no vacancy” button", .group = GRP(7), .keywords = EKW_1601, .skinToneSupport = false},
	EmojiData{ .emoji = "🔴", .name = "red circle", .group = GRP(7), .keywords = EKW_1602, .skinToneSupport = false},
	EmojiData{ .emoji = "🟠", .name = "orange circle", .group = GRP(7), .keywords = EKW_1603, .skinToneSupport = false},
	EmojiData{ .emoji = "🟡", .name = "yellow circle", .group = GRP(7), .keywords = EKW_1604, .skinToneSupport = false},
	EmojiData{ .emoji = "🟢", .name = "green circle", .group = GRP(7), .keywords = EKW_1605, .skinToneSupport = false},
	EmojiData{ .emoji = "🔵", .name = "blue circle", .group = GRP(7), .keywords = EKW_1606, .skinToneSupport = false},
	EmojiData{ .emoji = "🟣", .name = "purple circle", .group = GRP(7), .keywords = EKW_1607, .skinToneSupport = false},
	EmojiData{ .emoji = "🟤", .name = "brown circle", .group = GRP(7), .keywords = EKW_1608, .skinToneSupport = false},
	EmojiData{ .emoji = "⚫", .name = "black circle", .group = GRP(7), .keywords = EKW_1609, .skinToneSupport = false},
	EmojiData{ .emoji = "⚪", .name = "white circle", .group = GRP(7), .keywords = EKW_1610, .skinToneSupport = false},
	EmojiData{ .emoji = "🟥", .name = "red square", .group = GRP(7), .keywords = EKW_1611, .skinToneSupport = false},
	EmojiData{ .emoji = "🟧", .name = "orange square", .group = GRP(7), .keywords = EKW_1612, .skinToneSupport = false},
	EmojiData{ .emoji = "🟨", .name = "yellow square", .group = GRP(7), .keywords = EKW_1613, .skinToneSupport = false},
	EmojiData{ .emoji = "🟩", .name = "green square", .group = GRP(7), .keywords = EKW_1614, .skinToneSupport = false},
	EmojiData{ .emoji = "🟦", .name = "blue square", .group = GRP(7), .keywords = EKW_1615, .skinToneSupport = false},
	EmojiData{ .emoji = "🟪", .name = "purple square", .group = GRP(7), .keywords = EKW_1616, .skinToneSupport = false},
	EmojiData{ .emoji = "🟫", .name = "brown square", .group = GRP(7), .keywords = EKW_1617, .skinToneSupport = false},
	EmojiData{ .emoji = "⬛", .name = "black large square", .group = GRP(7), .keywords = EKW_1618, .skinToneSupport = false},
	EmojiData{ .emoji = "⬜", .name = "white large square", .group = GRP(7), .keywords = EKW_1619, .skinToneSupport = false},
	EmojiData{ .emoji = "◼️", .name = "black medium square", .group = GRP(7), .keywords = EKW_1620, .skinToneSupport = false},
	EmojiData{ .emoji = "◻️", .name = "white medium square", .group = GRP(7), .keywords = EKW_1621, .skinToneSupport = false},
	EmojiData{ .emoji = "◾", .name = "black medium-small square", .group = GRP(7), .keywords = EKW_1622, .skinToneSupport = false},
	EmojiData{ .emoji = "◽", .name = "white medium-small square", .group = GRP(7), .keywords = EKW_1623, .skinToneSupport = false},
	EmojiData{ .emoji = "▪️", .name = "black small square", .group = GRP(7), .keywords = EKW_1624, .skinToneSupport = false},
	EmojiData{ .emoji = "▫️", .name = "white small square", .group = GRP(7), .keywords = EKW_1625, .skinToneSupport = false},
	EmojiData{ .emoji = "🔶", .name = "large orange diamond", .group = GRP(7), .keywords = EKW_1626, .skinToneSupport = false},
	EmojiData{ .emoji = "🔷", .name = "large blue diamond", .group = GRP(7), .keywords = EKW_1627, .skinToneSupport = false},
	EmojiData{ .emoji = "🔸", .name = "small orange diamond", .group = GRP(7), .keywords = EKW_1628, .skinToneSupport = false},
	EmojiData{ .emoji = "🔹", .name = "small blue diamond", .group = GRP(7), .keywords = EKW_1629, .skinToneSupport = false},
	EmojiData{ .emoji = "🔺", .name = "red triangle pointed up", .group = GRP(7), .keywords = EKW_1630, .skinToneSupport = false},
	EmojiData{ .emoji = "🔻", .name = "red triangle pointed down", .group = GRP(7), .keywords = EKW_1631, .skinToneSupport = false},
	EmojiData{ .emoji = "💠", .name = "diamond with a dot", .group = GRP(7), .keywords = EKW_1632, .skinToneSupport = false},
	EmojiData{ .emoji = "🔘", .name = "radio button", .group = GRP(7), .keywords = EKW_1633, .skinToneSupport = false},
	EmojiData{ .emoji = "🔳", .name = "white square button", .group = GRP(7), .keywords = EKW_1634, .skinToneSupport = false},
	EmojiData{ .emoji = "🔲", .name = "black square button", .group = GRP(7), .keywords = EKW_1635, .skinToneSupport = false},
	EmojiData{ .emoji = "🏁", .name = "chequered flag", .group = GRP(8), .keywords = EKW_1636, .skinToneSupport = false},
	EmojiData{ .emoji = "🚩", .name = "triangular flag", .group = GRP(8), .keywords = EKW_1637, .skinToneSupport = false},
	EmojiData{ .emoji = "🎌", .name = "crossed flags", .group = GRP(8), .keywords = EKW_1638, .skinToneSupport = false},
	EmojiData{ .emoji = "🏴", .name = "black flag", .group = GRP(8), .keywords = EKW_1639, .skinToneSupport = false},
	EmojiData{ .emoji = "🏳️", .name = "white flag", .group = GRP(8), .keywords = EKW_1640, .skinToneSupport = false},
	EmojiData{ .emoji = "🏳️‍🌈", .name = "rainbow flag", .group = GRP(8), .keywords = EKW_1641, .skinToneSupport = false},
	EmojiData{ .emoji = "🏳️‍⚧️", .name = "transgender flag", .group = GRP(8), .keywords = EKW_1642, .skinToneSupport = false},
	EmojiData{ .emoji = "🏴‍☠️", .name = "pirate flag", .group = GRP(8), .keywords = EKW_1643, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇨", .name = "flag Ascension Island", .group = GRP(8), .keywords = EKW_1644, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇩", .name = "flag Andorra", .group = GRP(8), .keywords = EKW_1645, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇪", .name = "flag United Arab Emirates", .group = GRP(8), .keywords = EKW_1646, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇫", .name = "flag Afghanistan", .group = GRP(8), .keywords = EKW_1647, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇬", .name = "flag Antigua & Barbuda", .group = GRP(8), .keywords = EKW_1648, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇮", .name = "flag Anguilla", .group = GRP(8), .keywords = EKW_1649, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇱", .name = "flag Albania", .group = GRP(8), .keywords = EKW_1650, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇲", .name = "flag Armenia", .group = GRP(8), .keywords = EKW_1651, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇴", .name = "flag Angola", .group = GRP(8), .keywords = EKW_1652, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇶", .name = "flag Antarctica", .group = GRP(8), .keywords = EKW_1653, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇷", .name = "flag Argentina", .group = GRP(8), .keywords = EKW_1654, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇸", .name = "flag American Samoa", .group = GRP(8), .keywords = EKW_1655, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇹", .name = "flag Austria", .group = GRP(8), .keywords = EKW_1656, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇺", .name = "flag Australia", .group = GRP(8), .keywords = EKW_1657, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇼", .name = "flag Aruba", .group = GRP(8), .keywords = EKW_1658, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇽", .name = "flag Åland Islands", .group = GRP(8), .keywords = EKW_1659, .skinToneSupport = false},
	EmojiData{ .emoji = "🇦🇿", .name = "flag Azerbaijan", .group = GRP(8), .keywords = EKW_1660, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇦", .name = "flag Bosnia & Herzegovina", .group = GRP(8), .keywords = EKW_1661, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇧", .name = "flag Barbados", .group = GRP(8), .keywords = EKW_1662, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇩", .name = "flag Bangladesh", .group = GRP(8), .keywords = EKW_1663, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇪", .name = "flag Belgium", .group = GRP(8), .keywords = EKW_1664, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇫", .name = "flag Burkina Faso", .group = GRP(8), .keywords = EKW_1665, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇬", .name = "flag Bulgaria", .group = GRP(8), .keywords = EKW_1666, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇭", .name = "flag Bahrain", .group = GRP(8), .keywords = EKW_1667, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇮", .name = "flag Burundi", .group = GRP(8), .keywords = EKW_1668, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇯", .name = "flag Benin", .group = GRP(8), .keywords = EKW_1669, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇱", .name = "flag St. Barthélemy", .group = GRP(8), .keywords = EKW_1670, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇲", .name = "flag Bermuda", .group = GRP(8), .keywords = EKW_1671, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇳", .name = "flag Brunei", .group = GRP(8), .keywords = EKW_1672, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇴", .name = "flag Bolivia", .group = GRP(8), .keywords = EKW_1673, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇶", .name = "flag Caribbean Netherlands", .group = GRP(8), .keywords = EKW_1674, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇷", .name = "flag Brazil", .group = GRP(8), .keywords = EKW_1675, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇸", .name = "flag Bahamas", .group = GRP(8), .keywords = EKW_1676, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇹", .name = "flag Bhutan", .group = GRP(8), .keywords = EKW_1677, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇻", .name = "flag Bouvet Island", .group = GRP(8), .keywords = EKW_1678, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇼", .name = "flag Botswana", .group = GRP(8), .keywords = EKW_1679, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇾", .name = "flag Belarus", .group = GRP(8), .keywords = EKW_1680, .skinToneSupport = false},
	EmojiData{ .emoji = "🇧🇿", .name = "flag Belize", .group = GRP(8), .keywords = EKW_1681, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇦", .name = "flag Canada", .group = GRP(8), .keywords = EKW_1682, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇨", .name = "flag Cocos (Keeling) Islands", .group = GRP(8), .keywords = EKW_1683, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇩", .name = "flag Congo - Kinshasa", .group = GRP(8), .keywords = EKW_1684, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇫", .name = "flag Central African Republic", .group = GRP(8), .keywords = EKW_1685, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇬", .name = "flag Congo - Brazzaville", .group = GRP(8), .keywords = EKW_1686, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇭", .name = "flag Switzerland", .group = GRP(8), .keywords = EKW_1687, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇮", .name = "flag Côte d’Ivoire", .group = GRP(8), .keywords = EKW_1688, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇰", .name = "flag Cook Islands", .group = GRP(8), .keywords = EKW_1689, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇱", .name = "flag Chile", .group = GRP(8), .keywords = EKW_1690, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇲", .name = "flag Cameroon", .group = GRP(8), .keywords = EKW_1691, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇳", .name = "flag China", .group = GRP(8), .keywords = EKW_1692, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇴", .name = "flag Colombia", .group = GRP(8), .keywords = EKW_1693, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇵", .name = "flag Clipperton Island", .group = GRP(8), .keywords = EKW_1694, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇶", .name = "flag Sark", .group = GRP(8), .keywords = EKW_1695, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇷", .name = "flag Costa Rica", .group = GRP(8), .keywords = EKW_1696, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇺", .name = "flag Cuba", .group = GRP(8), .keywords = EKW_1697, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇻", .name = "flag Cape Verde", .group = GRP(8), .keywords = EKW_1698, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇼", .name = "flag Curaçao", .group = GRP(8), .keywords = EKW_1699, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇽", .name = "flag Christmas Island", .group = GRP(8), .keywords = EKW_1700, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇾", .name = "flag Cyprus", .group = GRP(8), .keywords = EKW_1701, .skinToneSupport = false},
	EmojiData{ .emoji = "🇨🇿", .name = "flag Czechia", .group = GRP(8), .keywords = EKW_1702, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇪", .name = "flag Germany", .group = GRP(8), .keywords = EKW_1703, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇬", .name = "flag Diego Garcia", .group = GRP(8), .keywords = EKW_1704, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇯", .name = "flag Djibouti", .group = GRP(8), .keywords = EKW_1705, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇰", .name = "flag Denmark", .group = GRP(8), .keywords = EKW_1706, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇲", .name = "flag Dominica", .group = GRP(8), .keywords = EKW_1707, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇴", .name = "flag Dominican Republic", .group = GRP(8), .keywords = EKW_1708, .skinToneSupport = false},
	EmojiData{ .emoji = "🇩🇿", .name = "flag Algeria", .group = GRP(8), .keywords = EKW_1709, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇦", .name = "flag Ceuta & Melilla", .group = GRP(8), .keywords = EKW_1710, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇨", .name = "flag Ecuador", .group = GRP(8), .keywords = EKW_1711, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇪", .name = "flag Estonia", .group = GRP(8), .keywords = EKW_1712, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇬", .name = "flag Egypt", .group = GRP(8), .keywords = EKW_1713, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇭", .name = "flag Western Sahara", .group = GRP(8), .keywords = EKW_1714, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇷", .name = "flag Eritrea", .group = GRP(8), .keywords = EKW_1715, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇸", .name = "flag Spain", .group = GRP(8), .keywords = EKW_1716, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇹", .name = "flag Ethiopia", .group = GRP(8), .keywords = EKW_1717, .skinToneSupport = false},
	EmojiData{ .emoji = "🇪🇺", .name = "flag European Union", .group = GRP(8), .keywords = EKW_1718, .skinToneSupport = false},
	EmojiData{ .emoji = "🇫🇮", .name = "flag Finland", .group = GRP(8), .keywords = EKW_1719, .skinToneSupport = false},
	EmojiData{ .emoji = "🇫🇯", .name = "flag Fiji", .group = GRP(8), .keywords = EKW_1720, .skinToneSupport = false},
	EmojiData{ .emoji = "🇫🇰", .name = "flag Falkland Islands", .group = GRP(8), .keywords = EKW_1721, .skinToneSupport = false},
	EmojiData{ .emoji = "🇫🇲", .name = "flag Micronesia", .group = GRP(8), .keywords = EKW_1722, .skinToneSupport = false},
	EmojiData{ .emoji = "🇫🇴", .name = "flag Faroe Islands", .group = GRP(8), .keywords = EKW_1723, .skinToneSupport = false},
	EmojiData{ .emoji = "🇫🇷", .name = "flag France", .group = GRP(8), .keywords = EKW_1724, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇦", .name = "flag Gabon", .group = GRP(8), .keywords = EKW_1725, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇧", .name = "flag United Kingdom", .group = GRP(8), .keywords = EKW_1726, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇩", .name = "flag Grenada", .group = GRP(8), .keywords = EKW_1727, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇪", .name = "flag Georgia", .group = GRP(8), .keywords = EKW_1728, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇫", .name = "flag French Guiana", .group = GRP(8), .keywords = EKW_1729, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇬", .name = "flag Guernsey", .group = GRP(8), .keywords = EKW_1730, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇭", .name = "flag Ghana", .group = GRP(8), .keywords = EKW_1731, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇮", .name = "flag Gibraltar", .group = GRP(8), .keywords = EKW_1732, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇱", .name = "flag Greenland", .group = GRP(8), .keywords = EKW_1733, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇲", .name = "flag Gambia", .group = GRP(8), .keywords = EKW_1734, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇳", .name = "flag Guinea", .group = GRP(8), .keywords = EKW_1735, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇵", .name = "flag Guadeloupe", .group = GRP(8), .keywords = EKW_1736, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇶", .name = "flag Equatorial Guinea", .group = GRP(8), .keywords = EKW_1737, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇷", .name = "flag Greece", .group = GRP(8), .keywords = EKW_1738, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇸", .name = "flag South Georgia & South Sandwich Islands", .group = GRP(8), .keywords = EKW_1739, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇹", .name = "flag Guatemala", .group = GRP(8), .keywords = EKW_1740, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇺", .name = "flag Guam", .group = GRP(8), .keywords = EKW_1741, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇼", .name = "flag Guinea-Bissau", .group = GRP(8), .keywords = EKW_1742, .skinToneSupport = false},
	EmojiData{ .emoji = "🇬🇾", .name = "flag Guyana", .group = GRP(8), .keywords = EKW_1743, .skinToneSupport = false},
	EmojiData{ .emoji = "🇭🇰", .name = "flag Hong Kong SAR China", .group = GRP(8), .keywords = EKW_1744, .skinToneSupport = false},
	EmojiData{ .emoji = "🇭🇲", .name = "flag Heard & McDonald Islands", .group = GRP(8), .keywords = EKW_1745, .skinToneSupport = false},
	EmojiData{ .emoji = "🇭🇳", .name = "flag Honduras", .group = GRP(8), .keywords = EKW_1746, .skinToneSupport = false},
	EmojiData{ .emoji = "🇭🇷", .name = "flag Croatia", .group = GRP(8), .keywords = EKW_1747, .skinToneSupport = false},
	EmojiData{ .emoji = "🇭🇹", .name = "flag Haiti", .group = GRP(8), .keywords = EKW_1748, .skinToneSupport = false},
	EmojiData{ .emoji = "🇭🇺", .name = "flag Hungary", .group = GRP(8), .keywords = EKW_1749, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇨", .name = "flag Canary Islands", .group = GRP(8), .keywords = EKW_1750, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇩", .name = "flag Indonesia", .group = GRP(8), .keywords = EKW_1751, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇪", .name = "flag Ireland", .group = GRP(8), .keywords = EKW_1752, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇱", .name = "flag Israel", .group = GRP(8), .keywords = EKW_1753, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇲", .name = "flag Isle of Man", .group = GRP(8), .keywords = EKW_1754, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇳", .name = "flag India", .group = GRP(8), .keywords = EKW_1755, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇴", .name = "flag British Indian Ocean Territory", .group = GRP(8), .keywords = EKW_1756, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇶", .name = "flag Iraq", .group = GRP(8), .keywords = EKW_1757, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇷", .name = "flag Iran", .group = GRP(8), .keywords = EKW_1758, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇸", .name = "flag Iceland", .group = GRP(8), .keywords = EKW_1759, .skinToneSupport = false},
	EmojiData{ .emoji = "🇮🇹", .name = "flag Italy", .group = GRP(8), .keywords = EKW_1760, .skinToneSupport = false},
	EmojiData{ .emoji = "🇯🇪", .name = "flag Jersey", .group = GRP(8), .keywords = EKW_1761, .skinToneSupport = false},
	EmojiData{ .emoji = "🇯🇲", .name = "flag Jamaica", .group = GRP(8), .keywords = EKW_1762, .skinToneSupport = false},
	EmojiData{ .emoji = "🇯🇴", .name = "flag Jordan", .group = GRP(8), .keywords = EKW_1763, .skinToneSupport = false},
	EmojiData{ .emoji = "🇯🇵", .name = "flag Japan", .group = GRP(8), .keywords = EKW_1764, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇪", .name = "flag Kenya", .group = GRP(8), .keywords = EKW_1765, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇬", .name = "flag Kyrgyzstan", .group = GRP(8), .keywords = EKW_1766, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇭", .name = "flag Cambodia", .group = GRP(8), .keywords = EKW_1767, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇮", .name = "flag Kiribati", .group = GRP(8), .keywords = EKW_1768, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇲", .name = "flag Comoros", .group = GRP(8), .keywords = EKW_1769, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇳", .name = "flag St. Kitts & Nevis", .group = GRP(8), .keywords = EKW_1770, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇵", .name = "flag North Korea", .group = GRP(8), .keywords = EKW_1771, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇷", .name = "flag South Korea", .group = GRP(8), .keywords = EKW_1772, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇼", .name = "flag Kuwait", .group = GRP(8), .keywords = EKW_1773, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇾", .name = "flag Cayman Islands", .group = GRP(8), .keywords = EKW_1774, .skinToneSupport = false},
	EmojiData{ .emoji = "🇰🇿", .name = "flag Kazakhstan", .group = GRP(8), .keywords = EKW_1775, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇦", .name = "flag Laos", .group = GRP(8), .keywords = EKW_1776, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇧", .name = "flag Lebanon", .group = GRP(8), .keywords = EKW_1777, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇨", .name = "flag St. Lucia", .group = GRP(8), .keywords = EKW_1778, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇮", .name = "flag Liechtenstein", .group = GRP(8), .keywords = EKW_1779, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇰", .name = "flag Sri Lanka", .group = GRP(8), .keywords = EKW_1780, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇷", .name = "flag Liberia", .group = GRP(8), .keywords = EKW_1781, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇸", .name = "flag Lesotho", .group = GRP(8), .keywords = EKW_1782, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇹", .name = "flag Lithuania", .group = GRP(8), .keywords = EKW_1783, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇺", .name = "flag Luxembourg", .group = GRP(8), .keywords = EKW_1784, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇻", .name = "flag Latvia", .group = GRP(8), .keywords = EKW_1785, .skinToneSupport = false},
	EmojiData{ .emoji = "🇱🇾", .name = "flag Libya", .group = GRP(8), .keywords = EKW_1786, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇦", .name = "flag Morocco", .group = GRP(8), .keywords = EKW_1787, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇨", .name = "flag Monaco", .group = GRP(8), .keywords = EKW_1788, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇩", .name = "flag Moldova", .group = GRP(8), .keywords = EKW_1789, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇪", .name = "flag Montenegro", .group = GRP(8), .keywords = EKW_1790, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇫", .name = "flag St. Martin", .group = GRP(8), .keywords = EKW_1791, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇬", .name = "flag Madagascar", .group = GRP(8), .keywords = EKW_1792, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇭", .name = "flag Marshall Islands", .group = GRP(8), .keywords = EKW_1793, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇰", .name = "flag North Macedonia", .group = GRP(8), .keywords = EKW_1794, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇱", .name = "flag Mali", .group = GRP(8), .keywords = EKW_1795, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇲", .name = "flag Myanmar (Burma)", .group = GRP(8), .keywords = EKW_1796, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇳", .name = "flag Mongolia", .group = GRP(8), .keywords = EKW_1797, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇴", .name = "flag Macao SAR China", .group = GRP(8), .keywords = EKW_1798, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇵", .name = "flag Northern Mariana Islands", .group = GRP(8), .keywords = EKW_1799, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇶", .name = "flag Martinique", .group = GRP(8), .keywords = EKW_1800, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇷", .name = "flag Mauritania", .group = GRP(8), .keywords = EKW_1801, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇸", .name = "flag Montserrat", .group = GRP(8), .keywords = EKW_1802, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇹", .name = "flag Malta", .group = GRP(8), .keywords = EKW_1803, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇺", .name = "flag Mauritius", .group = GRP(8), .keywords = EKW_1804, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇻", .name = "flag Maldives", .group = GRP(8), .keywords = EKW_1805, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇼", .name = "flag Malawi", .group = GRP(8), .keywords = EKW_1806, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇽", .name = "flag Mexico", .group = GRP(8), .keywords = EKW_1807, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇾", .name = "flag Malaysia", .group = GRP(8), .keywords = EKW_1808, .skinToneSupport = false},
	EmojiData{ .emoji = "🇲🇿", .name = "flag Mozambique", .group = GRP(8), .keywords = EKW_1809, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇦", .name = "flag Namibia", .group = GRP(8), .keywords = EKW_1810, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇨", .name = "flag New Caledonia", .group = GRP(8), .keywords = EKW_1811, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇪", .name = "flag Niger", .group = GRP(8), .keywords = EKW_1812, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇫", .name = "flag Norfolk Island", .group = GRP(8), .keywords = EKW_1813, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇬", .name = "flag Nigeria", .group = GRP(8), .keywords = EKW_1814, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇮", .name = "flag Nicaragua", .group = GRP(8), .keywords = EKW_1815, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇱", .name = "flag Netherlands", .group = GRP(8), .keywords = EKW_1816, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇴", .name = "flag Norway", .group = GRP(8), .keywords = EKW_1817, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇵", .name = "flag Nepal", .group = GRP(8), .keywords = EKW_1818, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇷", .name = "flag Nauru", .group = GRP(8), .keywords = EKW_1819, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇺", .name = "flag Niue", .group = GRP(8), .keywords = EKW_1820, .skinToneSupport = false},
	EmojiData{ .emoji = "🇳🇿", .name = "flag New Zealand", .group = GRP(8), .keywords = EKW_1821, .skinToneSupport = false},
	EmojiData{ .emoji = "🇴🇲", .name = "flag Oman", .group = GRP(8), .keywords = EKW_1822, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇦", .name = "flag Panama", .group = GRP(8), .keywords = EKW_1823, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇪", .name = "flag Peru", .group = GRP(8), .keywords = EKW_1824, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇫", .name = "flag French Polynesia", .group = GRP(8), .keywords = EKW_1825, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇬", .name = "flag Papua New Guinea", .group = GRP(8), .keywords = EKW_1826, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇭", .name = "flag Philippines", .group = GRP(8), .keywords = EKW_1827, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇰", .name = "flag Pakistan", .group = GRP(8), .keywords = EKW_1828, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇱", .name = "flag Poland", .group = GRP(8), .keywords = EKW_1829, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇲", .name = "flag St. Pierre & Miquelon", .group = GRP(8), .keywords = EKW_1830, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇳", .name = "flag Pitcairn Islands", .group = GRP(8), .keywords = EKW_1831, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇷", .name = "flag Puerto Rico", .group = GRP(8), .keywords = EKW_1832, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇸", .name = "flag Palestinian Territories", .group = GRP(8), .keywords = EKW_1833, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇹", .name = "flag Portugal", .group = GRP(8), .keywords = EKW_1834, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇼", .name = "flag Palau", .group = GRP(8), .keywords = EKW_1835, .skinToneSupport = false},
	EmojiData{ .emoji = "🇵🇾", .name = "flag Paraguay", .group = GRP(8), .keywords = EKW_1836, .skinToneSupport = false},
	EmojiData{ .emoji = "🇶🇦", .name = "flag Qatar", .group = GRP(8), .keywords = EKW_1837, .skinToneSupport = false},
	EmojiData{ .emoji = "🇷🇪", .name = "flag Réunion", .group = GRP(8), .keywords = EKW_1838, .skinToneSupport = false},
	EmojiData{ .emoji = "🇷🇴", .name = "flag Romania", .group = GRP(8), .keywords = EKW_1839, .skinToneSupport = false},
	EmojiData{ .emoji = "🇷🇸", .name = "flag Serbia", .group = GRP(8), .keywords = EKW_1840, .skinToneSupport = false},
	EmojiData{ .emoji = "🇷🇺", .name = "flag Russia", .group = GRP(8), .keywords = EKW_1841, .skinToneSupport = false},
	EmojiData{ .emoji = "🇷🇼", .name = "flag Rwanda", .group = GRP(8), .keywords = EKW_1842, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇦", .name = "flag Saudi Arabia", .group = GRP(8), .keywords = EKW_1843, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇧", .name = "flag Solomon Islands", .group = GRP(8), .keywords = EKW_1844, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇨", .name = "flag Seychelles", .group = GRP(8), .keywords = EKW_1845, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇩", .name = "flag Sudan", .group = GRP(8), .keywords = EKW_1846, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇪", .name = "flag Sweden", .group = GRP(8), .keywords = EKW_1847, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇬", .name = "flag Singapore", .group = GRP(8), .keywords = EKW_1848, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇭", .name = "flag St. Helena", .group = GRP(8), .keywords = EKW_1849, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇮", .name = "flag Slovenia", .group = GRP(8), .keywords = EKW_1850, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇯", .name = "flag Svalbard & Jan Mayen", .group = GRP(8), .keywords = EKW_1851, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇰", .name = "flag Slovakia", .group = GRP(8), .keywords = EKW_1852, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇱", .name = "flag Sierra Leone", .group = GRP(8), .keywords = EKW_1853, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇲", .name = "flag San Marino", .group = GRP(8), .keywords = EKW_1854, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇳", .name = "flag Senegal", .group = GRP(8), .keywords = EKW_1855, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇴", .name = "flag Somalia", .group = GRP(8), .keywords = EKW_1856, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇷", .name = "flag Suriname", .group = GRP(8), .keywords = EKW_1857, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇸", .name = "flag South Sudan", .group = GRP(8), .keywords = EKW_1858, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇹", .name = "flag São Tomé & Príncipe", .group = GRP(8), .keywords = EKW_1859, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇻", .name = "flag El Salvador", .group = GRP(8), .keywords = EKW_1860, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇽", .name = "flag Sint Maarten", .group = GRP(8), .keywords = EKW_1861, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇾", .name = "flag Syria", .group = GRP(8), .keywords = EKW_1862, .skinToneSupport = false},
	EmojiData{ .emoji = "🇸🇿", .name = "flag Eswatini", .group = GRP(8), .keywords = EKW_1863, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇦", .name = "flag Tristan da Cunha", .group = GRP(8), .keywords = EKW_1864, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇨", .name = "flag Turks & Caicos Islands", .group = GRP(8), .keywords = EKW_1865, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇩", .name = "flag Chad", .group = GRP(8), .keywords = EKW_1866, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇫", .name = "flag French Southern Territories", .group = GRP(8), .keywords = EKW_1867, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇬", .name = "flag Togo", .group = GRP(8), .keywords = EKW_1868, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇭", .name = "flag Thailand", .group = GRP(8), .keywords = EKW_1869, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇯", .name = "flag Tajikistan", .group = GRP(8), .keywords = EKW_1870, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇰", .name = "flag Tokelau", .group = GRP(8), .keywords = EKW_1871, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇱", .name = "flag Timor-Leste", .group = GRP(8), .keywords = EKW_1872, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇲", .name = "flag Turkmenistan", .group = GRP(8), .keywords = EKW_1873, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇳", .name = "flag Tunisia", .group = GRP(8), .keywords = EKW_1874, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇴", .name = "flag Tonga", .group = GRP(8), .keywords = EKW_1875, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇷", .name = "flag Türkiye", .group = GRP(8), .keywords = EKW_1876, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇹", .name = "flag Trinidad & Tobago", .group = GRP(8), .keywords = EKW_1877, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇻", .name = "flag Tuvalu", .group = GRP(8), .keywords = EKW_1878, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇼", .name = "flag Taiwan", .group = GRP(8), .keywords = EKW_1879, .skinToneSupport = false},
	EmojiData{ .emoji = "🇹🇿", .name = "flag Tanzania", .group = GRP(8), .keywords = EKW_1880, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇦", .name = "flag Ukraine", .group = GRP(8), .keywords = EKW_1881, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇬", .name = "flag Uganda", .group = GRP(8), .keywords = EKW_1882, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇲", .name = "flag U.S. Outlying Islands", .group = GRP(8), .keywords = EKW_1883, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇳", .name = "flag United Nations", .group = GRP(8), .keywords = EKW_1884, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇸", .name = "flag United States", .group = GRP(8), .keywords = EKW_1885, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇾", .name = "flag Uruguay", .group = GRP(8), .keywords = EKW_1886, .skinToneSupport = false},
	EmojiData{ .emoji = "🇺🇿", .name = "flag Uzbekistan", .group = GRP(8), .keywords = EKW_1887, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇦", .name = "flag Vatican City", .group = GRP(8), .keywords = EKW_1888, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇨", .name = "flag St. Vincent & Grenadines", .group = GRP(8), .keywords = EKW_1889, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇪", .name = "flag Venezuela", .group = GRP(8), .keywords = EKW_1890, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇬", .name = "flag British Virgin Islands", .group = GRP(8), .keywords = EKW_1891, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇮", .name = "flag U.S. Virgin Islands", .group = GRP(8), .keywords = EKW_1892, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇳", .name = "flag Vietnam", .group = GRP(8), .keywords = EKW_1893, .skinToneSupport = false},
	EmojiData{ .emoji = "🇻🇺", .name = "flag Vanuatu", .group = GRP(8), .keywords = EKW_1894, .skinToneSupport = false},
	EmojiData{ .emoji = "🇼🇫", .name = "flag Wallis & Futuna", .group = GRP(8), .keywords = EKW_1895, .skinToneSupport = false},
	EmojiData{ .emoji = "🇼🇸", .name = "flag Samoa", .group = GRP(8), .keywords = EKW_1896, .skinToneSupport = false},
	EmojiData{ .emoji = "🇽🇰", .name = "flag Kosovo", .group = GRP(8), .keywords = EKW_1897, .skinToneSupport = false},
	EmojiData{ .emoji = "🇾🇪", .name = "flag Yemen", .group = GRP(8), .keywords = EKW_1898, .skinToneSupport = false},
	EmojiData{ .emoji = "🇾🇹", .name = "flag Mayotte", .group = GRP(8), .keywords = EKW_1899, .skinToneSupport = false},
	EmojiData{ .emoji = "🇿🇦", .name = "flag South Africa", .group = GRP(8), .keywords = EKW_1900, .skinToneSupport = false},
	EmojiData{ .emoji = "🇿🇲", .name = "flag Zambia", .group = GRP(8), .keywords = EKW_1901, .skinToneSupport = false},
	EmojiData{ .emoji = "🇿🇼", .name = "flag Zimbabwe", .group = GRP(8), .keywords = EKW_1902, .skinToneSupport = false},
	EmojiData{ .emoji = "🏴󠁧󠁢󠁥󠁮󠁧󠁿", .name = "flag England", .group = GRP(8), .keywords = EKW_1903, .skinToneSupport = false},
	EmojiData{ .emoji = "🏴󠁧󠁢󠁳󠁣󠁴󠁿", .name = "flag Scotland", .group = GRP(8), .keywords = EKW_1904, .skinToneSupport = false},
	EmojiData{ .emoji = "🏴󠁧󠁢󠁷󠁬󠁳󠁿", .name = "flag Wales", .group = GRP(8), .keywords = EKW_1905, .skinToneSupport = false}
};

const std::array<EmojiData, 1906>& StaticEmojiDatabase::orderedList() { return EMOJI_LIST; }
const std::unordered_map<std::string_view, const EmojiData*> MAPPING = {
{ "😀", &EMOJI_LIST[0] },
{ "😃", &EMOJI_LIST[1] },
{ "😄", &EMOJI_LIST[2] },
{ "😁", &EMOJI_LIST[3] },
{ "😆", &EMOJI_LIST[4] },
{ "😅", &EMOJI_LIST[5] },
{ "🤣", &EMOJI_LIST[6] },
{ "😂", &EMOJI_LIST[7] },
{ "🙂", &EMOJI_LIST[8] },
{ "🙃", &EMOJI_LIST[9] },
{ "🫠", &EMOJI_LIST[10] },
{ "😉", &EMOJI_LIST[11] },
{ "😊", &EMOJI_LIST[12] },
{ "😇", &EMOJI_LIST[13] },
{ "🥰", &EMOJI_LIST[14] },
{ "😍", &EMOJI_LIST[15] },
{ "🤩", &EMOJI_LIST[16] },
{ "😘", &EMOJI_LIST[17] },
{ "😗", &EMOJI_LIST[18] },
{ "☺️", &EMOJI_LIST[19] },
{ "😚", &EMOJI_LIST[20] },
{ "😙", &EMOJI_LIST[21] },
{ "🥲", &EMOJI_LIST[22] },
{ "😋", &EMOJI_LIST[23] },
{ "😛", &EMOJI_LIST[24] },
{ "😜", &EMOJI_LIST[25] },
{ "🤪", &EMOJI_LIST[26] },
{ "😝", &EMOJI_LIST[27] },
{ "🤑", &EMOJI_LIST[28] },
{ "🤗", &EMOJI_LIST[29] },
{ "🤭", &EMOJI_LIST[30] },
{ "🫢", &EMOJI_LIST[31] },
{ "🫣", &EMOJI_LIST[32] },
{ "🤫", &EMOJI_LIST[33] },
{ "🤔", &EMOJI_LIST[34] },
{ "🫡", &EMOJI_LIST[35] },
{ "🤐", &EMOJI_LIST[36] },
{ "🤨", &EMOJI_LIST[37] },
{ "😐", &EMOJI_LIST[38] },
{ "😑", &EMOJI_LIST[39] },
{ "😶", &EMOJI_LIST[40] },
{ "🫥", &EMOJI_LIST[41] },
{ "😶‍🌫️", &EMOJI_LIST[42] },
{ "😏", &EMOJI_LIST[43] },
{ "😒", &EMOJI_LIST[44] },
{ "🙄", &EMOJI_LIST[45] },
{ "😬", &EMOJI_LIST[46] },
{ "😮‍💨", &EMOJI_LIST[47] },
{ "🤥", &EMOJI_LIST[48] },
{ "🫨", &EMOJI_LIST[49] },
{ "🙂‍↔️", &EMOJI_LIST[50] },
{ "🙂‍↕️", &EMOJI_LIST[51] },
{ "😌", &EMOJI_LIST[52] },
{ "😔", &EMOJI_LIST[53] },
{ "😪", &EMOJI_LIST[54] },
{ "🤤", &EMOJI_LIST[55] },
{ "😴", &EMOJI_LIST[56] },
{ "🫩", &EMOJI_LIST[57] },
{ "😷", &EMOJI_LIST[58] },
{ "🤒", &EMOJI_LIST[59] },
{ "🤕", &EMOJI_LIST[60] },
{ "🤢", &EMOJI_LIST[61] },
{ "🤮", &EMOJI_LIST[62] },
{ "🤧", &EMOJI_LIST[63] },
{ "🥵", &EMOJI_LIST[64] },
{ "🥶", &EMOJI_LIST[65] },
{ "🥴", &EMOJI_LIST[66] },
{ "😵", &EMOJI_LIST[67] },
{ "😵‍💫", &EMOJI_LIST[68] },
{ "🤯", &EMOJI_LIST[69] },
{ "🤠", &EMOJI_LIST[70] },
{ "🥳", &EMOJI_LIST[71] },
{ "🥸", &EMOJI_LIST[72] },
{ "😎", &EMOJI_LIST[73] },
{ "🤓", &EMOJI_LIST[74] },
{ "🧐", &EMOJI_LIST[75] },
{ "😕", &EMOJI_LIST[76] },
{ "🫤", &EMOJI_LIST[77] },
{ "😟", &EMOJI_LIST[78] },
{ "🙁", &EMOJI_LIST[79] },
{ "☹️", &EMOJI_LIST[80] },
{ "😮", &EMOJI_LIST[81] },
{ "😯", &EMOJI_LIST[82] },
{ "😲", &EMOJI_LIST[83] },
{ "😳", &EMOJI_LIST[84] },
{ "🥺", &EMOJI_LIST[85] },
{ "🥹", &EMOJI_LIST[86] },
{ "😦", &EMOJI_LIST[87] },
{ "😧", &EMOJI_LIST[88] },
{ "😨", &EMOJI_LIST[89] },
{ "😰", &EMOJI_LIST[90] },
{ "😥", &EMOJI_LIST[91] },
{ "😢", &EMOJI_LIST[92] },
{ "😭", &EMOJI_LIST[93] },
{ "😱", &EMOJI_LIST[94] },
{ "😖", &EMOJI_LIST[95] },
{ "😣", &EMOJI_LIST[96] },
{ "😞", &EMOJI_LIST[97] },
{ "😓", &EMOJI_LIST[98] },
{ "😩", &EMOJI_LIST[99] },
{ "😫", &EMOJI_LIST[100] },
{ "🥱", &EMOJI_LIST[101] },
{ "😤", &EMOJI_LIST[102] },
{ "😡", &EMOJI_LIST[103] },
{ "😠", &EMOJI_LIST[104] },
{ "🤬", &EMOJI_LIST[105] },
{ "😈", &EMOJI_LIST[106] },
{ "👿", &EMOJI_LIST[107] },
{ "💀", &EMOJI_LIST[108] },
{ "☠️", &EMOJI_LIST[109] },
{ "💩", &EMOJI_LIST[110] },
{ "🤡", &EMOJI_LIST[111] },
{ "👹", &EMOJI_LIST[112] },
{ "👺", &EMOJI_LIST[113] },
{ "👻", &EMOJI_LIST[114] },
{ "👽", &EMOJI_LIST[115] },
{ "👾", &EMOJI_LIST[116] },
{ "🤖", &EMOJI_LIST[117] },
{ "😺", &EMOJI_LIST[118] },
{ "😸", &EMOJI_LIST[119] },
{ "😹", &EMOJI_LIST[120] },
{ "😻", &EMOJI_LIST[121] },
{ "😼", &EMOJI_LIST[122] },
{ "😽", &EMOJI_LIST[123] },
{ "🙀", &EMOJI_LIST[124] },
{ "😿", &EMOJI_LIST[125] },
{ "😾", &EMOJI_LIST[126] },
{ "🙈", &EMOJI_LIST[127] },
{ "🙉", &EMOJI_LIST[128] },
{ "🙊", &EMOJI_LIST[129] },
{ "💌", &EMOJI_LIST[130] },
{ "💘", &EMOJI_LIST[131] },
{ "💝", &EMOJI_LIST[132] },
{ "💖", &EMOJI_LIST[133] },
{ "💗", &EMOJI_LIST[134] },
{ "💓", &EMOJI_LIST[135] },
{ "💞", &EMOJI_LIST[136] },
{ "💕", &EMOJI_LIST[137] },
{ "💟", &EMOJI_LIST[138] },
{ "❣️", &EMOJI_LIST[139] },
{ "💔", &EMOJI_LIST[140] },
{ "❤️‍🔥", &EMOJI_LIST[141] },
{ "❤️‍🩹", &EMOJI_LIST[142] },
{ "❤️", &EMOJI_LIST[143] },
{ "🩷", &EMOJI_LIST[144] },
{ "🧡", &EMOJI_LIST[145] },
{ "💛", &EMOJI_LIST[146] },
{ "💚", &EMOJI_LIST[147] },
{ "💙", &EMOJI_LIST[148] },
{ "🩵", &EMOJI_LIST[149] },
{ "💜", &EMOJI_LIST[150] },
{ "🤎", &EMOJI_LIST[151] },
{ "🖤", &EMOJI_LIST[152] },
{ "🩶", &EMOJI_LIST[153] },
{ "🤍", &EMOJI_LIST[154] },
{ "💋", &EMOJI_LIST[155] },
{ "💯", &EMOJI_LIST[156] },
{ "💢", &EMOJI_LIST[157] },
{ "💥", &EMOJI_LIST[158] },
{ "💫", &EMOJI_LIST[159] },
{ "💦", &EMOJI_LIST[160] },
{ "💨", &EMOJI_LIST[161] },
{ "🕳️", &EMOJI_LIST[162] },
{ "💬", &EMOJI_LIST[163] },
{ "👁️‍🗨️", &EMOJI_LIST[164] },
{ "🗨️", &EMOJI_LIST[165] },
{ "🗯️", &EMOJI_LIST[166] },
{ "💭", &EMOJI_LIST[167] },
{ "💤", &EMOJI_LIST[168] },
{ "👋", &EMOJI_LIST[169] },
{ "🤚", &EMOJI_LIST[170] },
{ "🖐️", &EMOJI_LIST[171] },
{ "✋", &EMOJI_LIST[172] },
{ "🖖", &EMOJI_LIST[173] },
{ "🫱", &EMOJI_LIST[174] },
{ "🫲", &EMOJI_LIST[175] },
{ "🫳", &EMOJI_LIST[176] },
{ "🫴", &EMOJI_LIST[177] },
{ "🫷", &EMOJI_LIST[178] },
{ "🫸", &EMOJI_LIST[179] },
{ "👌", &EMOJI_LIST[180] },
{ "🤌", &EMOJI_LIST[181] },
{ "🤏", &EMOJI_LIST[182] },
{ "✌️", &EMOJI_LIST[183] },
{ "🤞", &EMOJI_LIST[184] },
{ "🫰", &EMOJI_LIST[185] },
{ "🤟", &EMOJI_LIST[186] },
{ "🤘", &EMOJI_LIST[187] },
{ "🤙", &EMOJI_LIST[188] },
{ "👈", &EMOJI_LIST[189] },
{ "👉", &EMOJI_LIST[190] },
{ "👆", &EMOJI_LIST[191] },
{ "🖕", &EMOJI_LIST[192] },
{ "👇", &EMOJI_LIST[193] },
{ "☝️", &EMOJI_LIST[194] },
{ "🫵", &EMOJI_LIST[195] },
{ "👍", &EMOJI_LIST[196] },
{ "👎", &EMOJI_LIST[197] },
{ "✊", &EMOJI_LIST[198] },
{ "👊", &EMOJI_LIST[199] },
{ "🤛", &EMOJI_LIST[200] },
{ "🤜", &EMOJI_LIST[201] },
{ "👏", &EMOJI_LIST[202] },
{ "🙌", &EMOJI_LIST[203] },
{ "🫶", &EMOJI_LIST[204] },
{ "👐", &EMOJI_LIST[205] },
{ "🤲", &EMOJI_LIST[206] },
{ "🤝", &EMOJI_LIST[207] },
{ "🙏", &EMOJI_LIST[208] },
{ "✍️", &EMOJI_LIST[209] },
{ "💅", &EMOJI_LIST[210] },
{ "🤳", &EMOJI_LIST[211] },
{ "💪", &EMOJI_LIST[212] },
{ "🦾", &EMOJI_LIST[213] },
{ "🦿", &EMOJI_LIST[214] },
{ "🦵", &EMOJI_LIST[215] },
{ "🦶", &EMOJI_LIST[216] },
{ "👂", &EMOJI_LIST[217] },
{ "🦻", &EMOJI_LIST[218] },
{ "👃", &EMOJI_LIST[219] },
{ "🧠", &EMOJI_LIST[220] },
{ "🫀", &EMOJI_LIST[221] },
{ "🫁", &EMOJI_LIST[222] },
{ "🦷", &EMOJI_LIST[223] },
{ "🦴", &EMOJI_LIST[224] },
{ "👀", &EMOJI_LIST[225] },
{ "👁️", &EMOJI_LIST[226] },
{ "👅", &EMOJI_LIST[227] },
{ "👄", &EMOJI_LIST[228] },
{ "🫦", &EMOJI_LIST[229] },
{ "👶", &EMOJI_LIST[230] },
{ "🧒", &EMOJI_LIST[231] },
{ "👦", &EMOJI_LIST[232] },
{ "👧", &EMOJI_LIST[233] },
{ "🧑", &EMOJI_LIST[234] },
{ "👱", &EMOJI_LIST[235] },
{ "👨", &EMOJI_LIST[236] },
{ "🧔", &EMOJI_LIST[237] },
{ "🧔‍♂️", &EMOJI_LIST[238] },
{ "🧔‍♀️", &EMOJI_LIST[239] },
{ "👨‍🦰", &EMOJI_LIST[240] },
{ "👨‍🦱", &EMOJI_LIST[241] },
{ "👨‍🦳", &EMOJI_LIST[242] },
{ "👨‍🦲", &EMOJI_LIST[243] },
{ "👩", &EMOJI_LIST[244] },
{ "👩‍🦰", &EMOJI_LIST[245] },
{ "🧑‍🦰", &EMOJI_LIST[246] },
{ "👩‍🦱", &EMOJI_LIST[247] },
{ "🧑‍🦱", &EMOJI_LIST[248] },
{ "👩‍🦳", &EMOJI_LIST[249] },
{ "🧑‍🦳", &EMOJI_LIST[250] },
{ "👩‍🦲", &EMOJI_LIST[251] },
{ "🧑‍🦲", &EMOJI_LIST[252] },
{ "👱‍♀️", &EMOJI_LIST[253] },
{ "👱‍♂️", &EMOJI_LIST[254] },
{ "🧓", &EMOJI_LIST[255] },
{ "👴", &EMOJI_LIST[256] },
{ "👵", &EMOJI_LIST[257] },
{ "🙍", &EMOJI_LIST[258] },
{ "🙍‍♂️", &EMOJI_LIST[259] },
{ "🙍‍♀️", &EMOJI_LIST[260] },
{ "🙎", &EMOJI_LIST[261] },
{ "🙎‍♂️", &EMOJI_LIST[262] },
{ "🙎‍♀️", &EMOJI_LIST[263] },
{ "🙅", &EMOJI_LIST[264] },
{ "🙅‍♂️", &EMOJI_LIST[265] },
{ "🙅‍♀️", &EMOJI_LIST[266] },
{ "🙆", &EMOJI_LIST[267] },
{ "🙆‍♂️", &EMOJI_LIST[268] },
{ "🙆‍♀️", &EMOJI_LIST[269] },
{ "💁", &EMOJI_LIST[270] },
{ "💁‍♂️", &EMOJI_LIST[271] },
{ "💁‍♀️", &EMOJI_LIST[272] },
{ "🙋", &EMOJI_LIST[273] },
{ "🙋‍♂️", &EMOJI_LIST[274] },
{ "🙋‍♀️", &EMOJI_LIST[275] },
{ "🧏", &EMOJI_LIST[276] },
{ "🧏‍♂️", &EMOJI_LIST[277] },
{ "🧏‍♀️", &EMOJI_LIST[278] },
{ "🙇", &EMOJI_LIST[279] },
{ "🙇‍♂️", &EMOJI_LIST[280] },
{ "🙇‍♀️", &EMOJI_LIST[281] },
{ "🤦", &EMOJI_LIST[282] },
{ "🤦‍♂️", &EMOJI_LIST[283] },
{ "🤦‍♀️", &EMOJI_LIST[284] },
{ "🤷", &EMOJI_LIST[285] },
{ "🤷‍♂️", &EMOJI_LIST[286] },
{ "🤷‍♀️", &EMOJI_LIST[287] },
{ "🧑‍⚕️", &EMOJI_LIST[288] },
{ "👨‍⚕️", &EMOJI_LIST[289] },
{ "👩‍⚕️", &EMOJI_LIST[290] },
{ "🧑‍🎓", &EMOJI_LIST[291] },
{ "👨‍🎓", &EMOJI_LIST[292] },
{ "👩‍🎓", &EMOJI_LIST[293] },
{ "🧑‍🏫", &EMOJI_LIST[294] },
{ "👨‍🏫", &EMOJI_LIST[295] },
{ "👩‍🏫", &EMOJI_LIST[296] },
{ "🧑‍⚖️", &EMOJI_LIST[297] },
{ "👨‍⚖️", &EMOJI_LIST[298] },
{ "👩‍⚖️", &EMOJI_LIST[299] },
{ "🧑‍🌾", &EMOJI_LIST[300] },
{ "👨‍🌾", &EMOJI_LIST[301] },
{ "👩‍🌾", &EMOJI_LIST[302] },
{ "🧑‍🍳", &EMOJI_LIST[303] },
{ "👨‍🍳", &EMOJI_LIST[304] },
{ "👩‍🍳", &EMOJI_LIST[305] },
{ "🧑‍🔧", &EMOJI_LIST[306] },
{ "👨‍🔧", &EMOJI_LIST[307] },
{ "👩‍🔧", &EMOJI_LIST[308] },
{ "🧑‍🏭", &EMOJI_LIST[309] },
{ "👨‍🏭", &EMOJI_LIST[310] },
{ "👩‍🏭", &EMOJI_LIST[311] },
{ "🧑‍💼", &EMOJI_LIST[312] },
{ "👨‍💼", &EMOJI_LIST[313] },
{ "👩‍💼", &EMOJI_LIST[314] },
{ "🧑‍🔬", &EMOJI_LIST[315] },
{ "👨‍🔬", &EMOJI_LIST[316] },
{ "👩‍🔬", &EMOJI_LIST[317] },
{ "🧑‍💻", &EMOJI_LIST[318] },
{ "👨‍💻", &EMOJI_LIST[319] },
{ "👩‍💻", &EMOJI_LIST[320] },
{ "🧑‍🎤", &EMOJI_LIST[321] },
{ "👨‍🎤", &EMOJI_LIST[322] },
{ "👩‍🎤", &EMOJI_LIST[323] },
{ "🧑‍🎨", &EMOJI_LIST[324] },
{ "👨‍🎨", &EMOJI_LIST[325] },
{ "👩‍🎨", &EMOJI_LIST[326] },
{ "🧑‍✈️", &EMOJI_LIST[327] },
{ "👨‍✈️", &EMOJI_LIST[328] },
{ "👩‍✈️", &EMOJI_LIST[329] },
{ "🧑‍🚀", &EMOJI_LIST[330] },
{ "👨‍🚀", &EMOJI_LIST[331] },
{ "👩‍🚀", &EMOJI_LIST[332] },
{ "🧑‍🚒", &EMOJI_LIST[333] },
{ "👨‍🚒", &EMOJI_LIST[334] },
{ "👩‍🚒", &EMOJI_LIST[335] },
{ "👮", &EMOJI_LIST[336] },
{ "👮‍♂️", &EMOJI_LIST[337] },
{ "👮‍♀️", &EMOJI_LIST[338] },
{ "🕵️", &EMOJI_LIST[339] },
{ "🕵️‍♂️", &EMOJI_LIST[340] },
{ "🕵️‍♀️", &EMOJI_LIST[341] },
{ "💂", &EMOJI_LIST[342] },
{ "💂‍♂️", &EMOJI_LIST[343] },
{ "💂‍♀️", &EMOJI_LIST[344] },
{ "🥷", &EMOJI_LIST[345] },
{ "👷", &EMOJI_LIST[346] },
{ "👷‍♂️", &EMOJI_LIST[347] },
{ "👷‍♀️", &EMOJI_LIST[348] },
{ "🫅", &EMOJI_LIST[349] },
{ "🤴", &EMOJI_LIST[350] },
{ "👸", &EMOJI_LIST[351] },
{ "👳", &EMOJI_LIST[352] },
{ "👳‍♂️", &EMOJI_LIST[353] },
{ "👳‍♀️", &EMOJI_LIST[354] },
{ "👲", &EMOJI_LIST[355] },
{ "🧕", &EMOJI_LIST[356] },
{ "🤵", &EMOJI_LIST[357] },
{ "🤵‍♂️", &EMOJI_LIST[358] },
{ "🤵‍♀️", &EMOJI_LIST[359] },
{ "👰", &EMOJI_LIST[360] },
{ "👰‍♂️", &EMOJI_LIST[361] },
{ "👰‍♀️", &EMOJI_LIST[362] },
{ "🤰", &EMOJI_LIST[363] },
{ "🫃", &EMOJI_LIST[364] },
{ "🫄", &EMOJI_LIST[365] },
{ "🤱", &EMOJI_LIST[366] },
{ "👩‍🍼", &EMOJI_LIST[367] },
{ "👨‍🍼", &EMOJI_LIST[368] },
{ "🧑‍🍼", &EMOJI_LIST[369] },
{ "👼", &EMOJI_LIST[370] },
{ "🎅", &EMOJI_LIST[371] },
{ "🤶", &EMOJI_LIST[372] },
{ "🧑‍🎄", &EMOJI_LIST[373] },
{ "🦸", &EMOJI_LIST[374] },
{ "🦸‍♂️", &EMOJI_LIST[375] },
{ "🦸‍♀️", &EMOJI_LIST[376] },
{ "🦹", &EMOJI_LIST[377] },
{ "🦹‍♂️", &EMOJI_LIST[378] },
{ "🦹‍♀️", &EMOJI_LIST[379] },
{ "🧙", &EMOJI_LIST[380] },
{ "🧙‍♂️", &EMOJI_LIST[381] },
{ "🧙‍♀️", &EMOJI_LIST[382] },
{ "🧚", &EMOJI_LIST[383] },
{ "🧚‍♂️", &EMOJI_LIST[384] },
{ "🧚‍♀️", &EMOJI_LIST[385] },
{ "🧛", &EMOJI_LIST[386] },
{ "🧛‍♂️", &EMOJI_LIST[387] },
{ "🧛‍♀️", &EMOJI_LIST[388] },
{ "🧜", &EMOJI_LIST[389] },
{ "🧜‍♂️", &EMOJI_LIST[390] },
{ "🧜‍♀️", &EMOJI_LIST[391] },
{ "🧝", &EMOJI_LIST[392] },
{ "🧝‍♂️", &EMOJI_LIST[393] },
{ "🧝‍♀️", &EMOJI_LIST[394] },
{ "🧞", &EMOJI_LIST[395] },
{ "🧞‍♂️", &EMOJI_LIST[396] },
{ "🧞‍♀️", &EMOJI_LIST[397] },
{ "🧟", &EMOJI_LIST[398] },
{ "🧟‍♂️", &EMOJI_LIST[399] },
{ "🧟‍♀️", &EMOJI_LIST[400] },
{ "🧌", &EMOJI_LIST[401] },
{ "💆", &EMOJI_LIST[402] },
{ "💆‍♂️", &EMOJI_LIST[403] },
{ "💆‍♀️", &EMOJI_LIST[404] },
{ "💇", &EMOJI_LIST[405] },
{ "💇‍♂️", &EMOJI_LIST[406] },
{ "💇‍♀️", &EMOJI_LIST[407] },
{ "🚶", &EMOJI_LIST[408] },
{ "🚶‍♂️", &EMOJI_LIST[409] },
{ "🚶‍♀️", &EMOJI_LIST[410] },
{ "🚶‍➡️", &EMOJI_LIST[411] },
{ "🚶‍♀️‍➡️", &EMOJI_LIST[412] },
{ "🚶‍♂️‍➡️", &EMOJI_LIST[413] },
{ "🧍", &EMOJI_LIST[414] },
{ "🧍‍♂️", &EMOJI_LIST[415] },
{ "🧍‍♀️", &EMOJI_LIST[416] },
{ "🧎", &EMOJI_LIST[417] },
{ "🧎‍♂️", &EMOJI_LIST[418] },
{ "🧎‍♀️", &EMOJI_LIST[419] },
{ "🧎‍➡️", &EMOJI_LIST[420] },
{ "🧎‍♀️‍➡️", &EMOJI_LIST[421] },
{ "🧎‍♂️‍➡️", &EMOJI_LIST[422] },
{ "🧑‍🦯", &EMOJI_LIST[423] },
{ "🧑‍🦯‍➡️", &EMOJI_LIST[424] },
{ "👨‍🦯", &EMOJI_LIST[425] },
{ "👨‍🦯‍➡️", &EMOJI_LIST[426] },
{ "👩‍🦯", &EMOJI_LIST[427] },
{ "👩‍🦯‍➡️", &EMOJI_LIST[428] },
{ "🧑‍🦼", &EMOJI_LIST[429] },
{ "🧑‍🦼‍➡️", &EMOJI_LIST[430] },
{ "👨‍🦼", &EMOJI_LIST[431] },
{ "👨‍🦼‍➡️", &EMOJI_LIST[432] },
{ "👩‍🦼", &EMOJI_LIST[433] },
{ "👩‍🦼‍➡️", &EMOJI_LIST[434] },
{ "🧑‍🦽", &EMOJI_LIST[435] },
{ "🧑‍🦽‍➡️", &EMOJI_LIST[436] },
{ "👨‍🦽", &EMOJI_LIST[437] },
{ "👨‍🦽‍➡️", &EMOJI_LIST[438] },
{ "👩‍🦽", &EMOJI_LIST[439] },
{ "👩‍🦽‍➡️", &EMOJI_LIST[440] },
{ "🏃", &EMOJI_LIST[441] },
{ "🏃‍♂️", &EMOJI_LIST[442] },
{ "🏃‍♀️", &EMOJI_LIST[443] },
{ "🏃‍➡️", &EMOJI_LIST[444] },
{ "🏃‍♀️‍➡️", &EMOJI_LIST[445] },
{ "🏃‍♂️‍➡️", &EMOJI_LIST[446] },
{ "💃", &EMOJI_LIST[447] },
{ "🕺", &EMOJI_LIST[448] },
{ "🕴️", &EMOJI_LIST[449] },
{ "👯", &EMOJI_LIST[450] },
{ "👯‍♂️", &EMOJI_LIST[451] },
{ "👯‍♀️", &EMOJI_LIST[452] },
{ "🧖", &EMOJI_LIST[453] },
{ "🧖‍♂️", &EMOJI_LIST[454] },
{ "🧖‍♀️", &EMOJI_LIST[455] },
{ "🧗", &EMOJI_LIST[456] },
{ "🧗‍♂️", &EMOJI_LIST[457] },
{ "🧗‍♀️", &EMOJI_LIST[458] },
{ "🤺", &EMOJI_LIST[459] },
{ "🏇", &EMOJI_LIST[460] },
{ "⛷️", &EMOJI_LIST[461] },
{ "🏂", &EMOJI_LIST[462] },
{ "🏌️", &EMOJI_LIST[463] },
{ "🏌️‍♂️", &EMOJI_LIST[464] },
{ "🏌️‍♀️", &EMOJI_LIST[465] },
{ "🏄", &EMOJI_LIST[466] },
{ "🏄‍♂️", &EMOJI_LIST[467] },
{ "🏄‍♀️", &EMOJI_LIST[468] },
{ "🚣", &EMOJI_LIST[469] },
{ "🚣‍♂️", &EMOJI_LIST[470] },
{ "🚣‍♀️", &EMOJI_LIST[471] },
{ "🏊", &EMOJI_LIST[472] },
{ "🏊‍♂️", &EMOJI_LIST[473] },
{ "🏊‍♀️", &EMOJI_LIST[474] },
{ "⛹️", &EMOJI_LIST[475] },
{ "⛹️‍♂️", &EMOJI_LIST[476] },
{ "⛹️‍♀️", &EMOJI_LIST[477] },
{ "🏋️", &EMOJI_LIST[478] },
{ "🏋️‍♂️", &EMOJI_LIST[479] },
{ "🏋️‍♀️", &EMOJI_LIST[480] },
{ "🚴", &EMOJI_LIST[481] },
{ "🚴‍♂️", &EMOJI_LIST[482] },
{ "🚴‍♀️", &EMOJI_LIST[483] },
{ "🚵", &EMOJI_LIST[484] },
{ "🚵‍♂️", &EMOJI_LIST[485] },
{ "🚵‍♀️", &EMOJI_LIST[486] },
{ "🤸", &EMOJI_LIST[487] },
{ "🤸‍♂️", &EMOJI_LIST[488] },
{ "🤸‍♀️", &EMOJI_LIST[489] },
{ "🤼", &EMOJI_LIST[490] },
{ "🤼‍♂️", &EMOJI_LIST[491] },
{ "🤼‍♀️", &EMOJI_LIST[492] },
{ "🤽", &EMOJI_LIST[493] },
{ "🤽‍♂️", &EMOJI_LIST[494] },
{ "🤽‍♀️", &EMOJI_LIST[495] },
{ "🤾", &EMOJI_LIST[496] },
{ "🤾‍♂️", &EMOJI_LIST[497] },
{ "🤾‍♀️", &EMOJI_LIST[498] },
{ "🤹", &EMOJI_LIST[499] },
{ "🤹‍♂️", &EMOJI_LIST[500] },
{ "🤹‍♀️", &EMOJI_LIST[501] },
{ "🧘", &EMOJI_LIST[502] },
{ "🧘‍♂️", &EMOJI_LIST[503] },
{ "🧘‍♀️", &EMOJI_LIST[504] },
{ "🛀", &EMOJI_LIST[505] },
{ "🛌", &EMOJI_LIST[506] },
{ "🧑‍🤝‍🧑", &EMOJI_LIST[507] },
{ "👭", &EMOJI_LIST[508] },
{ "👫", &EMOJI_LIST[509] },
{ "👬", &EMOJI_LIST[510] },
{ "💏", &EMOJI_LIST[511] },
{ "👩‍❤️‍💋‍👨", &EMOJI_LIST[512] },
{ "👨‍❤️‍💋‍👨", &EMOJI_LIST[513] },
{ "👩‍❤️‍💋‍👩", &EMOJI_LIST[514] },
{ "💑", &EMOJI_LIST[515] },
{ "👩‍❤️‍👨", &EMOJI_LIST[516] },
{ "👨‍❤️‍👨", &EMOJI_LIST[517] },
{ "👩‍❤️‍👩", &EMOJI_LIST[518] },
{ "👨‍👩‍👦", &EMOJI_LIST[519] },
{ "👨‍👩‍👧", &EMOJI_LIST[520] },
{ "👨‍👩‍👧‍👦", &EMOJI_LIST[521] },
{ "👨‍👩‍👦‍👦", &EMOJI_LIST[522] },
{ "👨‍👩‍👧‍👧", &EMOJI_LIST[523] },
{ "👨‍👨‍👦", &EMOJI_LIST[524] },
{ "👨‍👨‍👧", &EMOJI_LIST[525] },
{ "👨‍👨‍👧‍👦", &EMOJI_LIST[526] },
{ "👨‍👨‍👦‍👦", &EMOJI_LIST[527] },
{ "👨‍👨‍👧‍👧", &EMOJI_LIST[528] },
{ "👩‍👩‍👦", &EMOJI_LIST[529] },
{ "👩‍👩‍👧", &EMOJI_LIST[530] },
{ "👩‍👩‍👧‍👦", &EMOJI_LIST[531] },
{ "👩‍👩‍👦‍👦", &EMOJI_LIST[532] },
{ "👩‍👩‍👧‍👧", &EMOJI_LIST[533] },
{ "👨‍👦", &EMOJI_LIST[534] },
{ "👨‍👦‍👦", &EMOJI_LIST[535] },
{ "👨‍👧", &EMOJI_LIST[536] },
{ "👨‍👧‍👦", &EMOJI_LIST[537] },
{ "👨‍👧‍👧", &EMOJI_LIST[538] },
{ "👩‍👦", &EMOJI_LIST[539] },
{ "👩‍👦‍👦", &EMOJI_LIST[540] },
{ "👩‍👧", &EMOJI_LIST[541] },
{ "👩‍👧‍👦", &EMOJI_LIST[542] },
{ "👩‍👧‍👧", &EMOJI_LIST[543] },
{ "🗣️", &EMOJI_LIST[544] },
{ "👤", &EMOJI_LIST[545] },
{ "👥", &EMOJI_LIST[546] },
{ "🫂", &EMOJI_LIST[547] },
{ "👪", &EMOJI_LIST[548] },
{ "🧑‍🧑‍🧒", &EMOJI_LIST[549] },
{ "🧑‍🧑‍🧒‍🧒", &EMOJI_LIST[550] },
{ "🧑‍🧒", &EMOJI_LIST[551] },
{ "🧑‍🧒‍🧒", &EMOJI_LIST[552] },
{ "👣", &EMOJI_LIST[553] },
{ "🫆", &EMOJI_LIST[554] },
{ "🐵", &EMOJI_LIST[555] },
{ "🐒", &EMOJI_LIST[556] },
{ "🦍", &EMOJI_LIST[557] },
{ "🦧", &EMOJI_LIST[558] },
{ "🐶", &EMOJI_LIST[559] },
{ "🐕", &EMOJI_LIST[560] },
{ "🦮", &EMOJI_LIST[561] },
{ "🐕‍🦺", &EMOJI_LIST[562] },
{ "🐩", &EMOJI_LIST[563] },
{ "🐺", &EMOJI_LIST[564] },
{ "🦊", &EMOJI_LIST[565] },
{ "🦝", &EMOJI_LIST[566] },
{ "🐱", &EMOJI_LIST[567] },
{ "🐈", &EMOJI_LIST[568] },
{ "🐈‍⬛", &EMOJI_LIST[569] },
{ "🦁", &EMOJI_LIST[570] },
{ "🐯", &EMOJI_LIST[571] },
{ "🐅", &EMOJI_LIST[572] },
{ "🐆", &EMOJI_LIST[573] },
{ "🐴", &EMOJI_LIST[574] },
{ "🫎", &EMOJI_LIST[575] },
{ "🫏", &EMOJI_LIST[576] },
{ "🐎", &EMOJI_LIST[577] },
{ "🦄", &EMOJI_LIST[578] },
{ "🦓", &EMOJI_LIST[579] },
{ "🦌", &EMOJI_LIST[580] },
{ "🦬", &EMOJI_LIST[581] },
{ "🐮", &EMOJI_LIST[582] },
{ "🐂", &EMOJI_LIST[583] },
{ "🐃", &EMOJI_LIST[584] },
{ "🐄", &EMOJI_LIST[585] },
{ "🐷", &EMOJI_LIST[586] },
{ "🐖", &EMOJI_LIST[587] },
{ "🐗", &EMOJI_LIST[588] },
{ "🐽", &EMOJI_LIST[589] },
{ "🐏", &EMOJI_LIST[590] },
{ "🐑", &EMOJI_LIST[591] },
{ "🐐", &EMOJI_LIST[592] },
{ "🐪", &EMOJI_LIST[593] },
{ "🐫", &EMOJI_LIST[594] },
{ "🦙", &EMOJI_LIST[595] },
{ "🦒", &EMOJI_LIST[596] },
{ "🐘", &EMOJI_LIST[597] },
{ "🦣", &EMOJI_LIST[598] },
{ "🦏", &EMOJI_LIST[599] },
{ "🦛", &EMOJI_LIST[600] },
{ "🐭", &EMOJI_LIST[601] },
{ "🐁", &EMOJI_LIST[602] },
{ "🐀", &EMOJI_LIST[603] },
{ "🐹", &EMOJI_LIST[604] },
{ "🐰", &EMOJI_LIST[605] },
{ "🐇", &EMOJI_LIST[606] },
{ "🐿️", &EMOJI_LIST[607] },
{ "🦫", &EMOJI_LIST[608] },
{ "🦔", &EMOJI_LIST[609] },
{ "🦇", &EMOJI_LIST[610] },
{ "🐻", &EMOJI_LIST[611] },
{ "🐻‍❄️", &EMOJI_LIST[612] },
{ "🐨", &EMOJI_LIST[613] },
{ "🐼", &EMOJI_LIST[614] },
{ "🦥", &EMOJI_LIST[615] },
{ "🦦", &EMOJI_LIST[616] },
{ "🦨", &EMOJI_LIST[617] },
{ "🦘", &EMOJI_LIST[618] },
{ "🦡", &EMOJI_LIST[619] },
{ "🐾", &EMOJI_LIST[620] },
{ "🦃", &EMOJI_LIST[621] },
{ "🐔", &EMOJI_LIST[622] },
{ "🐓", &EMOJI_LIST[623] },
{ "🐣", &EMOJI_LIST[624] },
{ "🐤", &EMOJI_LIST[625] },
{ "🐥", &EMOJI_LIST[626] },
{ "🐦", &EMOJI_LIST[627] },
{ "🐧", &EMOJI_LIST[628] },
{ "🕊️", &EMOJI_LIST[629] },
{ "🦅", &EMOJI_LIST[630] },
{ "🦆", &EMOJI_LIST[631] },
{ "🦢", &EMOJI_LIST[632] },
{ "🦉", &EMOJI_LIST[633] },
{ "🦤", &EMOJI_LIST[634] },
{ "🪶", &EMOJI_LIST[635] },
{ "🦩", &EMOJI_LIST[636] },
{ "🦚", &EMOJI_LIST[637] },
{ "🦜", &EMOJI_LIST[638] },
{ "🪽", &EMOJI_LIST[639] },
{ "🐦‍⬛", &EMOJI_LIST[640] },
{ "🪿", &EMOJI_LIST[641] },
{ "🐦‍🔥", &EMOJI_LIST[642] },
{ "🐸", &EMOJI_LIST[643] },
{ "🐊", &EMOJI_LIST[644] },
{ "🐢", &EMOJI_LIST[645] },
{ "🦎", &EMOJI_LIST[646] },
{ "🐍", &EMOJI_LIST[647] },
{ "🐲", &EMOJI_LIST[648] },
{ "🐉", &EMOJI_LIST[649] },
{ "🦕", &EMOJI_LIST[650] },
{ "🦖", &EMOJI_LIST[651] },
{ "🐳", &EMOJI_LIST[652] },
{ "🐋", &EMOJI_LIST[653] },
{ "🐬", &EMOJI_LIST[654] },
{ "🦭", &EMOJI_LIST[655] },
{ "🐟", &EMOJI_LIST[656] },
{ "🐠", &EMOJI_LIST[657] },
{ "🐡", &EMOJI_LIST[658] },
{ "🦈", &EMOJI_LIST[659] },
{ "🐙", &EMOJI_LIST[660] },
{ "🐚", &EMOJI_LIST[661] },
{ "🪸", &EMOJI_LIST[662] },
{ "🪼", &EMOJI_LIST[663] },
{ "🦀", &EMOJI_LIST[664] },
{ "🦞", &EMOJI_LIST[665] },
{ "🦐", &EMOJI_LIST[666] },
{ "🦑", &EMOJI_LIST[667] },
{ "🦪", &EMOJI_LIST[668] },
{ "🐌", &EMOJI_LIST[669] },
{ "🦋", &EMOJI_LIST[670] },
{ "🐛", &EMOJI_LIST[671] },
{ "🐜", &EMOJI_LIST[672] },
{ "🐝", &EMOJI_LIST[673] },
{ "🪲", &EMOJI_LIST[674] },
{ "🐞", &EMOJI_LIST[675] },
{ "🦗", &EMOJI_LIST[676] },
{ "🪳", &EMOJI_LIST[677] },
{ "🕷️", &EMOJI_LIST[678] },
{ "🕸️", &EMOJI_LIST[679] },
{ "🦂", &EMOJI_LIST[680] },
{ "🦟", &EMOJI_LIST[681] },
{ "🪰", &EMOJI_LIST[682] },
{ "🪱", &EMOJI_LIST[683] },
{ "🦠", &EMOJI_LIST[684] },
{ "💐", &EMOJI_LIST[685] },
{ "🌸", &EMOJI_LIST[686] },
{ "💮", &EMOJI_LIST[687] },
{ "🪷", &EMOJI_LIST[688] },
{ "🏵️", &EMOJI_LIST[689] },
{ "🌹", &EMOJI_LIST[690] },
{ "🥀", &EMOJI_LIST[691] },
{ "🌺", &EMOJI_LIST[692] },
{ "🌻", &EMOJI_LIST[693] },
{ "🌼", &EMOJI_LIST[694] },
{ "🌷", &EMOJI_LIST[695] },
{ "🪻", &EMOJI_LIST[696] },
{ "🌱", &EMOJI_LIST[697] },
{ "🪴", &EMOJI_LIST[698] },
{ "🌲", &EMOJI_LIST[699] },
{ "🌳", &EMOJI_LIST[700] },
{ "🌴", &EMOJI_LIST[701] },
{ "🌵", &EMOJI_LIST[702] },
{ "🌾", &EMOJI_LIST[703] },
{ "🌿", &EMOJI_LIST[704] },
{ "☘️", &EMOJI_LIST[705] },
{ "🍀", &EMOJI_LIST[706] },
{ "🍁", &EMOJI_LIST[707] },
{ "🍂", &EMOJI_LIST[708] },
{ "🍃", &EMOJI_LIST[709] },
{ "🪹", &EMOJI_LIST[710] },
{ "🪺", &EMOJI_LIST[711] },
{ "🍄", &EMOJI_LIST[712] },
{ "🪾", &EMOJI_LIST[713] },
{ "🍇", &EMOJI_LIST[714] },
{ "🍈", &EMOJI_LIST[715] },
{ "🍉", &EMOJI_LIST[716] },
{ "🍊", &EMOJI_LIST[717] },
{ "🍋", &EMOJI_LIST[718] },
{ "🍋‍🟩", &EMOJI_LIST[719] },
{ "🍌", &EMOJI_LIST[720] },
{ "🍍", &EMOJI_LIST[721] },
{ "🥭", &EMOJI_LIST[722] },
{ "🍎", &EMOJI_LIST[723] },
{ "🍏", &EMOJI_LIST[724] },
{ "🍐", &EMOJI_LIST[725] },
{ "🍑", &EMOJI_LIST[726] },
{ "🍒", &EMOJI_LIST[727] },
{ "🍓", &EMOJI_LIST[728] },
{ "🫐", &EMOJI_LIST[729] },
{ "🥝", &EMOJI_LIST[730] },
{ "🍅", &EMOJI_LIST[731] },
{ "🫒", &EMOJI_LIST[732] },
{ "🥥", &EMOJI_LIST[733] },
{ "🥑", &EMOJI_LIST[734] },
{ "🍆", &EMOJI_LIST[735] },
{ "🥔", &EMOJI_LIST[736] },
{ "🥕", &EMOJI_LIST[737] },
{ "🌽", &EMOJI_LIST[738] },
{ "🌶️", &EMOJI_LIST[739] },
{ "🫑", &EMOJI_LIST[740] },
{ "🥒", &EMOJI_LIST[741] },
{ "🥬", &EMOJI_LIST[742] },
{ "🥦", &EMOJI_LIST[743] },
{ "🧄", &EMOJI_LIST[744] },
{ "🧅", &EMOJI_LIST[745] },
{ "🥜", &EMOJI_LIST[746] },
{ "🫘", &EMOJI_LIST[747] },
{ "🌰", &EMOJI_LIST[748] },
{ "🫚", &EMOJI_LIST[749] },
{ "🫛", &EMOJI_LIST[750] },
{ "🍄‍🟫", &EMOJI_LIST[751] },
{ "🫜", &EMOJI_LIST[752] },
{ "🍞", &EMOJI_LIST[753] },
{ "🥐", &EMOJI_LIST[754] },
{ "🥖", &EMOJI_LIST[755] },
{ "🫓", &EMOJI_LIST[756] },
{ "🥨", &EMOJI_LIST[757] },
{ "🥯", &EMOJI_LIST[758] },
{ "🥞", &EMOJI_LIST[759] },
{ "🧇", &EMOJI_LIST[760] },
{ "🧀", &EMOJI_LIST[761] },
{ "🍖", &EMOJI_LIST[762] },
{ "🍗", &EMOJI_LIST[763] },
{ "🥩", &EMOJI_LIST[764] },
{ "🥓", &EMOJI_LIST[765] },
{ "🍔", &EMOJI_LIST[766] },
{ "🍟", &EMOJI_LIST[767] },
{ "🍕", &EMOJI_LIST[768] },
{ "🌭", &EMOJI_LIST[769] },
{ "🥪", &EMOJI_LIST[770] },
{ "🌮", &EMOJI_LIST[771] },
{ "🌯", &EMOJI_LIST[772] },
{ "🫔", &EMOJI_LIST[773] },
{ "🥙", &EMOJI_LIST[774] },
{ "🧆", &EMOJI_LIST[775] },
{ "🥚", &EMOJI_LIST[776] },
{ "🍳", &EMOJI_LIST[777] },
{ "🥘", &EMOJI_LIST[778] },
{ "🍲", &EMOJI_LIST[779] },
{ "🫕", &EMOJI_LIST[780] },
{ "🥣", &EMOJI_LIST[781] },
{ "🥗", &EMOJI_LIST[782] },
{ "🍿", &EMOJI_LIST[783] },
{ "🧈", &EMOJI_LIST[784] },
{ "🧂", &EMOJI_LIST[785] },
{ "🥫", &EMOJI_LIST[786] },
{ "🍱", &EMOJI_LIST[787] },
{ "🍘", &EMOJI_LIST[788] },
{ "🍙", &EMOJI_LIST[789] },
{ "🍚", &EMOJI_LIST[790] },
{ "🍛", &EMOJI_LIST[791] },
{ "🍜", &EMOJI_LIST[792] },
{ "🍝", &EMOJI_LIST[793] },
{ "🍠", &EMOJI_LIST[794] },
{ "🍢", &EMOJI_LIST[795] },
{ "🍣", &EMOJI_LIST[796] },
{ "🍤", &EMOJI_LIST[797] },
{ "🍥", &EMOJI_LIST[798] },
{ "🥮", &EMOJI_LIST[799] },
{ "🍡", &EMOJI_LIST[800] },
{ "🥟", &EMOJI_LIST[801] },
{ "🥠", &EMOJI_LIST[802] },
{ "🥡", &EMOJI_LIST[803] },
{ "🍦", &EMOJI_LIST[804] },
{ "🍧", &EMOJI_LIST[805] },
{ "🍨", &EMOJI_LIST[806] },
{ "🍩", &EMOJI_LIST[807] },
{ "🍪", &EMOJI_LIST[808] },
{ "🎂", &EMOJI_LIST[809] },
{ "🍰", &EMOJI_LIST[810] },
{ "🧁", &EMOJI_LIST[811] },
{ "🥧", &EMOJI_LIST[812] },
{ "🍫", &EMOJI_LIST[813] },
{ "🍬", &EMOJI_LIST[814] },
{ "🍭", &EMOJI_LIST[815] },
{ "🍮", &EMOJI_LIST[816] },
{ "🍯", &EMOJI_LIST[817] },
{ "🍼", &EMOJI_LIST[818] },
{ "🥛", &EMOJI_LIST[819] },
{ "☕", &EMOJI_LIST[820] },
{ "🫖", &EMOJI_LIST[821] },
{ "🍵", &EMOJI_LIST[822] },
{ "🍶", &EMOJI_LIST[823] },
{ "🍾", &EMOJI_LIST[824] },
{ "🍷", &EMOJI_LIST[825] },
{ "🍸", &EMOJI_LIST[826] },
{ "🍹", &EMOJI_LIST[827] },
{ "🍺", &EMOJI_LIST[828] },
{ "🍻", &EMOJI_LIST[829] },
{ "🥂", &EMOJI_LIST[830] },
{ "🥃", &EMOJI_LIST[831] },
{ "🫗", &EMOJI_LIST[832] },
{ "🥤", &EMOJI_LIST[833] },
{ "🧋", &EMOJI_LIST[834] },
{ "🧃", &EMOJI_LIST[835] },
{ "🧉", &EMOJI_LIST[836] },
{ "🧊", &EMOJI_LIST[837] },
{ "🥢", &EMOJI_LIST[838] },
{ "🍽️", &EMOJI_LIST[839] },
{ "🍴", &EMOJI_LIST[840] },
{ "🥄", &EMOJI_LIST[841] },
{ "🔪", &EMOJI_LIST[842] },
{ "🫙", &EMOJI_LIST[843] },
{ "🏺", &EMOJI_LIST[844] },
{ "🌍", &EMOJI_LIST[845] },
{ "🌎", &EMOJI_LIST[846] },
{ "🌏", &EMOJI_LIST[847] },
{ "🌐", &EMOJI_LIST[848] },
{ "🗺️", &EMOJI_LIST[849] },
{ "🗾", &EMOJI_LIST[850] },
{ "🧭", &EMOJI_LIST[851] },
{ "🏔️", &EMOJI_LIST[852] },
{ "⛰️", &EMOJI_LIST[853] },
{ "🌋", &EMOJI_LIST[854] },
{ "🗻", &EMOJI_LIST[855] },
{ "🏕️", &EMOJI_LIST[856] },
{ "🏖️", &EMOJI_LIST[857] },
{ "🏜️", &EMOJI_LIST[858] },
{ "🏝️", &EMOJI_LIST[859] },
{ "🏞️", &EMOJI_LIST[860] },
{ "🏟️", &EMOJI_LIST[861] },
{ "🏛️", &EMOJI_LIST[862] },
{ "🏗️", &EMOJI_LIST[863] },
{ "🧱", &EMOJI_LIST[864] },
{ "🪨", &EMOJI_LIST[865] },
{ "🪵", &EMOJI_LIST[866] },
{ "🛖", &EMOJI_LIST[867] },
{ "🏘️", &EMOJI_LIST[868] },
{ "🏚️", &EMOJI_LIST[869] },
{ "🏠", &EMOJI_LIST[870] },
{ "🏡", &EMOJI_LIST[871] },
{ "🏢", &EMOJI_LIST[872] },
{ "🏣", &EMOJI_LIST[873] },
{ "🏤", &EMOJI_LIST[874] },
{ "🏥", &EMOJI_LIST[875] },
{ "🏦", &EMOJI_LIST[876] },
{ "🏨", &EMOJI_LIST[877] },
{ "🏩", &EMOJI_LIST[878] },
{ "🏪", &EMOJI_LIST[879] },
{ "🏫", &EMOJI_LIST[880] },
{ "🏬", &EMOJI_LIST[881] },
{ "🏭", &EMOJI_LIST[882] },
{ "🏯", &EMOJI_LIST[883] },
{ "🏰", &EMOJI_LIST[884] },
{ "💒", &EMOJI_LIST[885] },
{ "🗼", &EMOJI_LIST[886] },
{ "🗽", &EMOJI_LIST[887] },
{ "⛪", &EMOJI_LIST[888] },
{ "🕌", &EMOJI_LIST[889] },
{ "🛕", &EMOJI_LIST[890] },
{ "🕍", &EMOJI_LIST[891] },
{ "⛩️", &EMOJI_LIST[892] },
{ "🕋", &EMOJI_LIST[893] },
{ "⛲", &EMOJI_LIST[894] },
{ "⛺", &EMOJI_LIST[895] },
{ "🌁", &EMOJI_LIST[896] },
{ "🌃", &EMOJI_LIST[897] },
{ "🏙️", &EMOJI_LIST[898] },
{ "🌄", &EMOJI_LIST[899] },
{ "🌅", &EMOJI_LIST[900] },
{ "🌆", &EMOJI_LIST[901] },
{ "🌇", &EMOJI_LIST[902] },
{ "🌉", &EMOJI_LIST[903] },
{ "♨️", &EMOJI_LIST[904] },
{ "🎠", &EMOJI_LIST[905] },
{ "🛝", &EMOJI_LIST[906] },
{ "🎡", &EMOJI_LIST[907] },
{ "🎢", &EMOJI_LIST[908] },
{ "💈", &EMOJI_LIST[909] },
{ "🎪", &EMOJI_LIST[910] },
{ "🚂", &EMOJI_LIST[911] },
{ "🚃", &EMOJI_LIST[912] },
{ "🚄", &EMOJI_LIST[913] },
{ "🚅", &EMOJI_LIST[914] },
{ "🚆", &EMOJI_LIST[915] },
{ "🚇", &EMOJI_LIST[916] },
{ "🚈", &EMOJI_LIST[917] },
{ "🚉", &EMOJI_LIST[918] },
{ "🚊", &EMOJI_LIST[919] },
{ "🚝", &EMOJI_LIST[920] },
{ "🚞", &EMOJI_LIST[921] },
{ "🚋", &EMOJI_LIST[922] },
{ "🚌", &EMOJI_LIST[923] },
{ "🚍", &EMOJI_LIST[924] },
{ "🚎", &EMOJI_LIST[925] },
{ "🚐", &EMOJI_LIST[926] },
{ "🚑", &EMOJI_LIST[927] },
{ "🚒", &EMOJI_LIST[928] },
{ "🚓", &EMOJI_LIST[929] },
{ "🚔", &EMOJI_LIST[930] },
{ "🚕", &EMOJI_LIST[931] },
{ "🚖", &EMOJI_LIST[932] },
{ "🚗", &EMOJI_LIST[933] },
{ "🚘", &EMOJI_LIST[934] },
{ "🚙", &EMOJI_LIST[935] },
{ "🛻", &EMOJI_LIST[936] },
{ "🚚", &EMOJI_LIST[937] },
{ "🚛", &EMOJI_LIST[938] },
{ "🚜", &EMOJI_LIST[939] },
{ "🏎️", &EMOJI_LIST[940] },
{ "🏍️", &EMOJI_LIST[941] },
{ "🛵", &EMOJI_LIST[942] },
{ "🦽", &EMOJI_LIST[943] },
{ "🦼", &EMOJI_LIST[944] },
{ "🛺", &EMOJI_LIST[945] },
{ "🚲", &EMOJI_LIST[946] },
{ "🛴", &EMOJI_LIST[947] },
{ "🛹", &EMOJI_LIST[948] },
{ "🛼", &EMOJI_LIST[949] },
{ "🚏", &EMOJI_LIST[950] },
{ "🛣️", &EMOJI_LIST[951] },
{ "🛤️", &EMOJI_LIST[952] },
{ "🛢️", &EMOJI_LIST[953] },
{ "⛽", &EMOJI_LIST[954] },
{ "🛞", &EMOJI_LIST[955] },
{ "🚨", &EMOJI_LIST[956] },
{ "🚥", &EMOJI_LIST[957] },
{ "🚦", &EMOJI_LIST[958] },
{ "🛑", &EMOJI_LIST[959] },
{ "🚧", &EMOJI_LIST[960] },
{ "⚓", &EMOJI_LIST[961] },
{ "🛟", &EMOJI_LIST[962] },
{ "⛵", &EMOJI_LIST[963] },
{ "🛶", &EMOJI_LIST[964] },
{ "🚤", &EMOJI_LIST[965] },
{ "🛳️", &EMOJI_LIST[966] },
{ "⛴️", &EMOJI_LIST[967] },
{ "🛥️", &EMOJI_LIST[968] },
{ "🚢", &EMOJI_LIST[969] },
{ "✈️", &EMOJI_LIST[970] },
{ "🛩️", &EMOJI_LIST[971] },
{ "🛫", &EMOJI_LIST[972] },
{ "🛬", &EMOJI_LIST[973] },
{ "🪂", &EMOJI_LIST[974] },
{ "💺", &EMOJI_LIST[975] },
{ "🚁", &EMOJI_LIST[976] },
{ "🚟", &EMOJI_LIST[977] },
{ "🚠", &EMOJI_LIST[978] },
{ "🚡", &EMOJI_LIST[979] },
{ "🛰️", &EMOJI_LIST[980] },
{ "🚀", &EMOJI_LIST[981] },
{ "🛸", &EMOJI_LIST[982] },
{ "🛎️", &EMOJI_LIST[983] },
{ "🧳", &EMOJI_LIST[984] },
{ "⌛", &EMOJI_LIST[985] },
{ "⏳", &EMOJI_LIST[986] },
{ "⌚", &EMOJI_LIST[987] },
{ "⏰", &EMOJI_LIST[988] },
{ "⏱️", &EMOJI_LIST[989] },
{ "⏲️", &EMOJI_LIST[990] },
{ "🕰️", &EMOJI_LIST[991] },
{ "🕛", &EMOJI_LIST[992] },
{ "🕧", &EMOJI_LIST[993] },
{ "🕐", &EMOJI_LIST[994] },
{ "🕜", &EMOJI_LIST[995] },
{ "🕑", &EMOJI_LIST[996] },
{ "🕝", &EMOJI_LIST[997] },
{ "🕒", &EMOJI_LIST[998] },
{ "🕞", &EMOJI_LIST[999] },
{ "🕓", &EMOJI_LIST[1000] },
{ "🕟", &EMOJI_LIST[1001] },
{ "🕔", &EMOJI_LIST[1002] },
{ "🕠", &EMOJI_LIST[1003] },
{ "🕕", &EMOJI_LIST[1004] },
{ "🕡", &EMOJI_LIST[1005] },
{ "🕖", &EMOJI_LIST[1006] },
{ "🕢", &EMOJI_LIST[1007] },
{ "🕗", &EMOJI_LIST[1008] },
{ "🕣", &EMOJI_LIST[1009] },
{ "🕘", &EMOJI_LIST[1010] },
{ "🕤", &EMOJI_LIST[1011] },
{ "🕙", &EMOJI_LIST[1012] },
{ "🕥", &EMOJI_LIST[1013] },
{ "🕚", &EMOJI_LIST[1014] },
{ "🕦", &EMOJI_LIST[1015] },
{ "🌑", &EMOJI_LIST[1016] },
{ "🌒", &EMOJI_LIST[1017] },
{ "🌓", &EMOJI_LIST[1018] },
{ "🌔", &EMOJI_LIST[1019] },
{ "🌕", &EMOJI_LIST[1020] },
{ "🌖", &EMOJI_LIST[1021] },
{ "🌗", &EMOJI_LIST[1022] },
{ "🌘", &EMOJI_LIST[1023] },
{ "🌙", &EMOJI_LIST[1024] },
{ "🌚", &EMOJI_LIST[1025] },
{ "🌛", &EMOJI_LIST[1026] },
{ "🌜", &EMOJI_LIST[1027] },
{ "🌡️", &EMOJI_LIST[1028] },
{ "☀️", &EMOJI_LIST[1029] },
{ "🌝", &EMOJI_LIST[1030] },
{ "🌞", &EMOJI_LIST[1031] },
{ "🪐", &EMOJI_LIST[1032] },
{ "⭐", &EMOJI_LIST[1033] },
{ "🌟", &EMOJI_LIST[1034] },
{ "🌠", &EMOJI_LIST[1035] },
{ "🌌", &EMOJI_LIST[1036] },
{ "☁️", &EMOJI_LIST[1037] },
{ "⛅", &EMOJI_LIST[1038] },
{ "⛈️", &EMOJI_LIST[1039] },
{ "🌤️", &EMOJI_LIST[1040] },
{ "🌥️", &EMOJI_LIST[1041] },
{ "🌦️", &EMOJI_LIST[1042] },
{ "🌧️", &EMOJI_LIST[1043] },
{ "🌨️", &EMOJI_LIST[1044] },
{ "🌩️", &EMOJI_LIST[1045] },
{ "🌪️", &EMOJI_LIST[1046] },
{ "🌫️", &EMOJI_LIST[1047] },
{ "🌬️", &EMOJI_LIST[1048] },
{ "🌀", &EMOJI_LIST[1049] },
{ "🌈", &EMOJI_LIST[1050] },
{ "🌂", &EMOJI_LIST[1051] },
{ "☂️", &EMOJI_LIST[1052] },
{ "☔", &EMOJI_LIST[1053] },
{ "⛱️", &EMOJI_LIST[1054] },
{ "⚡", &EMOJI_LIST[1055] },
{ "❄️", &EMOJI_LIST[1056] },
{ "☃️", &EMOJI_LIST[1057] },
{ "⛄", &EMOJI_LIST[1058] },
{ "☄️", &EMOJI_LIST[1059] },
{ "🔥", &EMOJI_LIST[1060] },
{ "💧", &EMOJI_LIST[1061] },
{ "🌊", &EMOJI_LIST[1062] },
{ "🎃", &EMOJI_LIST[1063] },
{ "🎄", &EMOJI_LIST[1064] },
{ "🎆", &EMOJI_LIST[1065] },
{ "🎇", &EMOJI_LIST[1066] },
{ "🧨", &EMOJI_LIST[1067] },
{ "✨", &EMOJI_LIST[1068] },
{ "🎈", &EMOJI_LIST[1069] },
{ "🎉", &EMOJI_LIST[1070] },
{ "🎊", &EMOJI_LIST[1071] },
{ "🎋", &EMOJI_LIST[1072] },
{ "🎍", &EMOJI_LIST[1073] },
{ "🎎", &EMOJI_LIST[1074] },
{ "🎏", &EMOJI_LIST[1075] },
{ "🎐", &EMOJI_LIST[1076] },
{ "🎑", &EMOJI_LIST[1077] },
{ "🧧", &EMOJI_LIST[1078] },
{ "🎀", &EMOJI_LIST[1079] },
{ "🎁", &EMOJI_LIST[1080] },
{ "🎗️", &EMOJI_LIST[1081] },
{ "🎟️", &EMOJI_LIST[1082] },
{ "🎫", &EMOJI_LIST[1083] },
{ "🎖️", &EMOJI_LIST[1084] },
{ "🏆", &EMOJI_LIST[1085] },
{ "🏅", &EMOJI_LIST[1086] },
{ "🥇", &EMOJI_LIST[1087] },
{ "🥈", &EMOJI_LIST[1088] },
{ "🥉", &EMOJI_LIST[1089] },
{ "⚽", &EMOJI_LIST[1090] },
{ "⚾", &EMOJI_LIST[1091] },
{ "🥎", &EMOJI_LIST[1092] },
{ "🏀", &EMOJI_LIST[1093] },
{ "🏐", &EMOJI_LIST[1094] },
{ "🏈", &EMOJI_LIST[1095] },
{ "🏉", &EMOJI_LIST[1096] },
{ "🎾", &EMOJI_LIST[1097] },
{ "🥏", &EMOJI_LIST[1098] },
{ "🎳", &EMOJI_LIST[1099] },
{ "🏏", &EMOJI_LIST[1100] },
{ "🏑", &EMOJI_LIST[1101] },
{ "🏒", &EMOJI_LIST[1102] },
{ "🥍", &EMOJI_LIST[1103] },
{ "🏓", &EMOJI_LIST[1104] },
{ "🏸", &EMOJI_LIST[1105] },
{ "🥊", &EMOJI_LIST[1106] },
{ "🥋", &EMOJI_LIST[1107] },
{ "🥅", &EMOJI_LIST[1108] },
{ "⛳", &EMOJI_LIST[1109] },
{ "⛸️", &EMOJI_LIST[1110] },
{ "🎣", &EMOJI_LIST[1111] },
{ "🤿", &EMOJI_LIST[1112] },
{ "🎽", &EMOJI_LIST[1113] },
{ "🎿", &EMOJI_LIST[1114] },
{ "🛷", &EMOJI_LIST[1115] },
{ "🥌", &EMOJI_LIST[1116] },
{ "🎯", &EMOJI_LIST[1117] },
{ "🪀", &EMOJI_LIST[1118] },
{ "🪁", &EMOJI_LIST[1119] },
{ "🔫", &EMOJI_LIST[1120] },
{ "🎱", &EMOJI_LIST[1121] },
{ "🔮", &EMOJI_LIST[1122] },
{ "🪄", &EMOJI_LIST[1123] },
{ "🎮", &EMOJI_LIST[1124] },
{ "🕹️", &EMOJI_LIST[1125] },
{ "🎰", &EMOJI_LIST[1126] },
{ "🎲", &EMOJI_LIST[1127] },
{ "🧩", &EMOJI_LIST[1128] },
{ "🧸", &EMOJI_LIST[1129] },
{ "🪅", &EMOJI_LIST[1130] },
{ "🪩", &EMOJI_LIST[1131] },
{ "🪆", &EMOJI_LIST[1132] },
{ "♠️", &EMOJI_LIST[1133] },
{ "♥️", &EMOJI_LIST[1134] },
{ "♦️", &EMOJI_LIST[1135] },
{ "♣️", &EMOJI_LIST[1136] },
{ "♟️", &EMOJI_LIST[1137] },
{ "🃏", &EMOJI_LIST[1138] },
{ "🀄", &EMOJI_LIST[1139] },
{ "🎴", &EMOJI_LIST[1140] },
{ "🎭", &EMOJI_LIST[1141] },
{ "🖼️", &EMOJI_LIST[1142] },
{ "🎨", &EMOJI_LIST[1143] },
{ "🧵", &EMOJI_LIST[1144] },
{ "🪡", &EMOJI_LIST[1145] },
{ "🧶", &EMOJI_LIST[1146] },
{ "🪢", &EMOJI_LIST[1147] },
{ "👓", &EMOJI_LIST[1148] },
{ "🕶️", &EMOJI_LIST[1149] },
{ "🥽", &EMOJI_LIST[1150] },
{ "🥼", &EMOJI_LIST[1151] },
{ "🦺", &EMOJI_LIST[1152] },
{ "👔", &EMOJI_LIST[1153] },
{ "👕", &EMOJI_LIST[1154] },
{ "👖", &EMOJI_LIST[1155] },
{ "🧣", &EMOJI_LIST[1156] },
{ "🧤", &EMOJI_LIST[1157] },
{ "🧥", &EMOJI_LIST[1158] },
{ "🧦", &EMOJI_LIST[1159] },
{ "👗", &EMOJI_LIST[1160] },
{ "👘", &EMOJI_LIST[1161] },
{ "🥻", &EMOJI_LIST[1162] },
{ "🩱", &EMOJI_LIST[1163] },
{ "🩲", &EMOJI_LIST[1164] },
{ "🩳", &EMOJI_LIST[1165] },
{ "👙", &EMOJI_LIST[1166] },
{ "👚", &EMOJI_LIST[1167] },
{ "🪭", &EMOJI_LIST[1168] },
{ "👛", &EMOJI_LIST[1169] },
{ "👜", &EMOJI_LIST[1170] },
{ "👝", &EMOJI_LIST[1171] },
{ "🛍️", &EMOJI_LIST[1172] },
{ "🎒", &EMOJI_LIST[1173] },
{ "🩴", &EMOJI_LIST[1174] },
{ "👞", &EMOJI_LIST[1175] },
{ "👟", &EMOJI_LIST[1176] },
{ "🥾", &EMOJI_LIST[1177] },
{ "🥿", &EMOJI_LIST[1178] },
{ "👠", &EMOJI_LIST[1179] },
{ "👡", &EMOJI_LIST[1180] },
{ "🩰", &EMOJI_LIST[1181] },
{ "👢", &EMOJI_LIST[1182] },
{ "🪮", &EMOJI_LIST[1183] },
{ "👑", &EMOJI_LIST[1184] },
{ "👒", &EMOJI_LIST[1185] },
{ "🎩", &EMOJI_LIST[1186] },
{ "🎓", &EMOJI_LIST[1187] },
{ "🧢", &EMOJI_LIST[1188] },
{ "🪖", &EMOJI_LIST[1189] },
{ "⛑️", &EMOJI_LIST[1190] },
{ "📿", &EMOJI_LIST[1191] },
{ "💄", &EMOJI_LIST[1192] },
{ "💍", &EMOJI_LIST[1193] },
{ "💎", &EMOJI_LIST[1194] },
{ "🔇", &EMOJI_LIST[1195] },
{ "🔈", &EMOJI_LIST[1196] },
{ "🔉", &EMOJI_LIST[1197] },
{ "🔊", &EMOJI_LIST[1198] },
{ "📢", &EMOJI_LIST[1199] },
{ "📣", &EMOJI_LIST[1200] },
{ "📯", &EMOJI_LIST[1201] },
{ "🔔", &EMOJI_LIST[1202] },
{ "🔕", &EMOJI_LIST[1203] },
{ "🎼", &EMOJI_LIST[1204] },
{ "🎵", &EMOJI_LIST[1205] },
{ "🎶", &EMOJI_LIST[1206] },
{ "🎙️", &EMOJI_LIST[1207] },
{ "🎚️", &EMOJI_LIST[1208] },
{ "🎛️", &EMOJI_LIST[1209] },
{ "🎤", &EMOJI_LIST[1210] },
{ "🎧", &EMOJI_LIST[1211] },
{ "📻", &EMOJI_LIST[1212] },
{ "🎷", &EMOJI_LIST[1213] },
{ "🪗", &EMOJI_LIST[1214] },
{ "🎸", &EMOJI_LIST[1215] },
{ "🎹", &EMOJI_LIST[1216] },
{ "🎺", &EMOJI_LIST[1217] },
{ "🎻", &EMOJI_LIST[1218] },
{ "🪕", &EMOJI_LIST[1219] },
{ "🥁", &EMOJI_LIST[1220] },
{ "🪘", &EMOJI_LIST[1221] },
{ "🪇", &EMOJI_LIST[1222] },
{ "🪈", &EMOJI_LIST[1223] },
{ "🪉", &EMOJI_LIST[1224] },
{ "📱", &EMOJI_LIST[1225] },
{ "📲", &EMOJI_LIST[1226] },
{ "☎️", &EMOJI_LIST[1227] },
{ "📞", &EMOJI_LIST[1228] },
{ "📟", &EMOJI_LIST[1229] },
{ "📠", &EMOJI_LIST[1230] },
{ "🔋", &EMOJI_LIST[1231] },
{ "🪫", &EMOJI_LIST[1232] },
{ "🔌", &EMOJI_LIST[1233] },
{ "💻", &EMOJI_LIST[1234] },
{ "🖥️", &EMOJI_LIST[1235] },
{ "🖨️", &EMOJI_LIST[1236] },
{ "⌨️", &EMOJI_LIST[1237] },
{ "🖱️", &EMOJI_LIST[1238] },
{ "🖲️", &EMOJI_LIST[1239] },
{ "💽", &EMOJI_LIST[1240] },
{ "💾", &EMOJI_LIST[1241] },
{ "💿", &EMOJI_LIST[1242] },
{ "📀", &EMOJI_LIST[1243] },
{ "🧮", &EMOJI_LIST[1244] },
{ "🎥", &EMOJI_LIST[1245] },
{ "🎞️", &EMOJI_LIST[1246] },
{ "📽️", &EMOJI_LIST[1247] },
{ "🎬", &EMOJI_LIST[1248] },
{ "📺", &EMOJI_LIST[1249] },
{ "📷", &EMOJI_LIST[1250] },
{ "📸", &EMOJI_LIST[1251] },
{ "📹", &EMOJI_LIST[1252] },
{ "📼", &EMOJI_LIST[1253] },
{ "🔍", &EMOJI_LIST[1254] },
{ "🔎", &EMOJI_LIST[1255] },
{ "🕯️", &EMOJI_LIST[1256] },
{ "💡", &EMOJI_LIST[1257] },
{ "🔦", &EMOJI_LIST[1258] },
{ "🏮", &EMOJI_LIST[1259] },
{ "🪔", &EMOJI_LIST[1260] },
{ "📔", &EMOJI_LIST[1261] },
{ "📕", &EMOJI_LIST[1262] },
{ "📖", &EMOJI_LIST[1263] },
{ "📗", &EMOJI_LIST[1264] },
{ "📘", &EMOJI_LIST[1265] },
{ "📙", &EMOJI_LIST[1266] },
{ "📚", &EMOJI_LIST[1267] },
{ "📓", &EMOJI_LIST[1268] },
{ "📒", &EMOJI_LIST[1269] },
{ "📃", &EMOJI_LIST[1270] },
{ "📜", &EMOJI_LIST[1271] },
{ "📄", &EMOJI_LIST[1272] },
{ "📰", &EMOJI_LIST[1273] },
{ "🗞️", &EMOJI_LIST[1274] },
{ "📑", &EMOJI_LIST[1275] },
{ "🔖", &EMOJI_LIST[1276] },
{ "🏷️", &EMOJI_LIST[1277] },
{ "💰", &EMOJI_LIST[1278] },
{ "🪙", &EMOJI_LIST[1279] },
{ "💴", &EMOJI_LIST[1280] },
{ "💵", &EMOJI_LIST[1281] },
{ "💶", &EMOJI_LIST[1282] },
{ "💷", &EMOJI_LIST[1283] },
{ "💸", &EMOJI_LIST[1284] },
{ "💳", &EMOJI_LIST[1285] },
{ "🧾", &EMOJI_LIST[1286] },
{ "💹", &EMOJI_LIST[1287] },
{ "✉️", &EMOJI_LIST[1288] },
{ "📧", &EMOJI_LIST[1289] },
{ "📨", &EMOJI_LIST[1290] },
{ "📩", &EMOJI_LIST[1291] },
{ "📤", &EMOJI_LIST[1292] },
{ "📥", &EMOJI_LIST[1293] },
{ "📦", &EMOJI_LIST[1294] },
{ "📫", &EMOJI_LIST[1295] },
{ "📪", &EMOJI_LIST[1296] },
{ "📬", &EMOJI_LIST[1297] },
{ "📭", &EMOJI_LIST[1298] },
{ "📮", &EMOJI_LIST[1299] },
{ "🗳️", &EMOJI_LIST[1300] },
{ "✏️", &EMOJI_LIST[1301] },
{ "✒️", &EMOJI_LIST[1302] },
{ "🖋️", &EMOJI_LIST[1303] },
{ "🖊️", &EMOJI_LIST[1304] },
{ "🖌️", &EMOJI_LIST[1305] },
{ "🖍️", &EMOJI_LIST[1306] },
{ "📝", &EMOJI_LIST[1307] },
{ "💼", &EMOJI_LIST[1308] },
{ "📁", &EMOJI_LIST[1309] },
{ "📂", &EMOJI_LIST[1310] },
{ "🗂️", &EMOJI_LIST[1311] },
{ "📅", &EMOJI_LIST[1312] },
{ "📆", &EMOJI_LIST[1313] },
{ "🗒️", &EMOJI_LIST[1314] },
{ "🗓️", &EMOJI_LIST[1315] },
{ "📇", &EMOJI_LIST[1316] },
{ "📈", &EMOJI_LIST[1317] },
{ "📉", &EMOJI_LIST[1318] },
{ "📊", &EMOJI_LIST[1319] },
{ "📋", &EMOJI_LIST[1320] },
{ "📌", &EMOJI_LIST[1321] },
{ "📍", &EMOJI_LIST[1322] },
{ "📎", &EMOJI_LIST[1323] },
{ "🖇️", &EMOJI_LIST[1324] },
{ "📏", &EMOJI_LIST[1325] },
{ "📐", &EMOJI_LIST[1326] },
{ "✂️", &EMOJI_LIST[1327] },
{ "🗃️", &EMOJI_LIST[1328] },
{ "🗄️", &EMOJI_LIST[1329] },
{ "🗑️", &EMOJI_LIST[1330] },
{ "🔒", &EMOJI_LIST[1331] },
{ "🔓", &EMOJI_LIST[1332] },
{ "🔏", &EMOJI_LIST[1333] },
{ "🔐", &EMOJI_LIST[1334] },
{ "🔑", &EMOJI_LIST[1335] },
{ "🗝️", &EMOJI_LIST[1336] },
{ "🔨", &EMOJI_LIST[1337] },
{ "🪓", &EMOJI_LIST[1338] },
{ "⛏️", &EMOJI_LIST[1339] },
{ "⚒️", &EMOJI_LIST[1340] },
{ "🛠️", &EMOJI_LIST[1341] },
{ "🗡️", &EMOJI_LIST[1342] },
{ "⚔️", &EMOJI_LIST[1343] },
{ "💣", &EMOJI_LIST[1344] },
{ "🪃", &EMOJI_LIST[1345] },
{ "🏹", &EMOJI_LIST[1346] },
{ "🛡️", &EMOJI_LIST[1347] },
{ "🪚", &EMOJI_LIST[1348] },
{ "🔧", &EMOJI_LIST[1349] },
{ "🪛", &EMOJI_LIST[1350] },
{ "🔩", &EMOJI_LIST[1351] },
{ "⚙️", &EMOJI_LIST[1352] },
{ "🗜️", &EMOJI_LIST[1353] },
{ "⚖️", &EMOJI_LIST[1354] },
{ "🦯", &EMOJI_LIST[1355] },
{ "🔗", &EMOJI_LIST[1356] },
{ "⛓️‍💥", &EMOJI_LIST[1357] },
{ "⛓️", &EMOJI_LIST[1358] },
{ "🪝", &EMOJI_LIST[1359] },
{ "🧰", &EMOJI_LIST[1360] },
{ "🧲", &EMOJI_LIST[1361] },
{ "🪜", &EMOJI_LIST[1362] },
{ "🪏", &EMOJI_LIST[1363] },
{ "⚗️", &EMOJI_LIST[1364] },
{ "🧪", &EMOJI_LIST[1365] },
{ "🧫", &EMOJI_LIST[1366] },
{ "🧬", &EMOJI_LIST[1367] },
{ "🔬", &EMOJI_LIST[1368] },
{ "🔭", &EMOJI_LIST[1369] },
{ "📡", &EMOJI_LIST[1370] },
{ "💉", &EMOJI_LIST[1371] },
{ "🩸", &EMOJI_LIST[1372] },
{ "💊", &EMOJI_LIST[1373] },
{ "🩹", &EMOJI_LIST[1374] },
{ "🩼", &EMOJI_LIST[1375] },
{ "🩺", &EMOJI_LIST[1376] },
{ "🩻", &EMOJI_LIST[1377] },
{ "🚪", &EMOJI_LIST[1378] },
{ "🛗", &EMOJI_LIST[1379] },
{ "🪞", &EMOJI_LIST[1380] },
{ "🪟", &EMOJI_LIST[1381] },
{ "🛏️", &EMOJI_LIST[1382] },
{ "🛋️", &EMOJI_LIST[1383] },
{ "🪑", &EMOJI_LIST[1384] },
{ "🚽", &EMOJI_LIST[1385] },
{ "🪠", &EMOJI_LIST[1386] },
{ "🚿", &EMOJI_LIST[1387] },
{ "🛁", &EMOJI_LIST[1388] },
{ "🪤", &EMOJI_LIST[1389] },
{ "🪒", &EMOJI_LIST[1390] },
{ "🧴", &EMOJI_LIST[1391] },
{ "🧷", &EMOJI_LIST[1392] },
{ "🧹", &EMOJI_LIST[1393] },
{ "🧺", &EMOJI_LIST[1394] },
{ "🧻", &EMOJI_LIST[1395] },
{ "🪣", &EMOJI_LIST[1396] },
{ "🧼", &EMOJI_LIST[1397] },
{ "🫧", &EMOJI_LIST[1398] },
{ "🪥", &EMOJI_LIST[1399] },
{ "🧽", &EMOJI_LIST[1400] },
{ "🧯", &EMOJI_LIST[1401] },
{ "🛒", &EMOJI_LIST[1402] },
{ "🚬", &EMOJI_LIST[1403] },
{ "⚰️", &EMOJI_LIST[1404] },
{ "🪦", &EMOJI_LIST[1405] },
{ "⚱️", &EMOJI_LIST[1406] },
{ "🧿", &EMOJI_LIST[1407] },
{ "🪬", &EMOJI_LIST[1408] },
{ "🗿", &EMOJI_LIST[1409] },
{ "🪧", &EMOJI_LIST[1410] },
{ "🪪", &EMOJI_LIST[1411] },
{ "🏧", &EMOJI_LIST[1412] },
{ "🚮", &EMOJI_LIST[1413] },
{ "🚰", &EMOJI_LIST[1414] },
{ "♿", &EMOJI_LIST[1415] },
{ "🚹", &EMOJI_LIST[1416] },
{ "🚺", &EMOJI_LIST[1417] },
{ "🚻", &EMOJI_LIST[1418] },
{ "🚼", &EMOJI_LIST[1419] },
{ "🚾", &EMOJI_LIST[1420] },
{ "🛂", &EMOJI_LIST[1421] },
{ "🛃", &EMOJI_LIST[1422] },
{ "🛄", &EMOJI_LIST[1423] },
{ "🛅", &EMOJI_LIST[1424] },
{ "⚠️", &EMOJI_LIST[1425] },
{ "🚸", &EMOJI_LIST[1426] },
{ "⛔", &EMOJI_LIST[1427] },
{ "🚫", &EMOJI_LIST[1428] },
{ "🚳", &EMOJI_LIST[1429] },
{ "🚭", &EMOJI_LIST[1430] },
{ "🚯", &EMOJI_LIST[1431] },
{ "🚱", &EMOJI_LIST[1432] },
{ "🚷", &EMOJI_LIST[1433] },
{ "📵", &EMOJI_LIST[1434] },
{ "🔞", &EMOJI_LIST[1435] },
{ "☢️", &EMOJI_LIST[1436] },
{ "☣️", &EMOJI_LIST[1437] },
{ "⬆️", &EMOJI_LIST[1438] },
{ "↗️", &EMOJI_LIST[1439] },
{ "➡️", &EMOJI_LIST[1440] },
{ "↘️", &EMOJI_LIST[1441] },
{ "⬇️", &EMOJI_LIST[1442] },
{ "↙️", &EMOJI_LIST[1443] },
{ "⬅️", &EMOJI_LIST[1444] },
{ "↖️", &EMOJI_LIST[1445] },
{ "↕️", &EMOJI_LIST[1446] },
{ "↔️", &EMOJI_LIST[1447] },
{ "↩️", &EMOJI_LIST[1448] },
{ "↪️", &EMOJI_LIST[1449] },
{ "⤴️", &EMOJI_LIST[1450] },
{ "⤵️", &EMOJI_LIST[1451] },
{ "🔃", &EMOJI_LIST[1452] },
{ "🔄", &EMOJI_LIST[1453] },
{ "🔙", &EMOJI_LIST[1454] },
{ "🔚", &EMOJI_LIST[1455] },
{ "🔛", &EMOJI_LIST[1456] },
{ "🔜", &EMOJI_LIST[1457] },
{ "🔝", &EMOJI_LIST[1458] },
{ "🛐", &EMOJI_LIST[1459] },
{ "⚛️", &EMOJI_LIST[1460] },
{ "🕉️", &EMOJI_LIST[1461] },
{ "✡️", &EMOJI_LIST[1462] },
{ "☸️", &EMOJI_LIST[1463] },
{ "☯️", &EMOJI_LIST[1464] },
{ "✝️", &EMOJI_LIST[1465] },
{ "☦️", &EMOJI_LIST[1466] },
{ "☪️", &EMOJI_LIST[1467] },
{ "☮️", &EMOJI_LIST[1468] },
{ "🕎", &EMOJI_LIST[1469] },
{ "🔯", &EMOJI_LIST[1470] },
{ "🪯", &EMOJI_LIST[1471] },
{ "♈", &EMOJI_LIST[1472] },
{ "♉", &EMOJI_LIST[1473] },
{ "♊", &EMOJI_LIST[1474] },
{ "♋", &EMOJI_LIST[1475] },
{ "♌", &EMOJI_LIST[1476] },
{ "♍", &EMOJI_LIST[1477] },
{ "♎", &EMOJI_LIST[1478] },
{ "♏", &EMOJI_LIST[1479] },
{ "♐", &EMOJI_LIST[1480] },
{ "♑", &EMOJI_LIST[1481] },
{ "♒", &EMOJI_LIST[1482] },
{ "♓", &EMOJI_LIST[1483] },
{ "⛎", &EMOJI_LIST[1484] },
{ "🔀", &EMOJI_LIST[1485] },
{ "🔁", &EMOJI_LIST[1486] },
{ "🔂", &EMOJI_LIST[1487] },
{ "▶️", &EMOJI_LIST[1488] },
{ "⏩", &EMOJI_LIST[1489] },
{ "⏭️", &EMOJI_LIST[1490] },
{ "⏯️", &EMOJI_LIST[1491] },
{ "◀️", &EMOJI_LIST[1492] },
{ "⏪", &EMOJI_LIST[1493] },
{ "⏮️", &EMOJI_LIST[1494] },
{ "🔼", &EMOJI_LIST[1495] },
{ "⏫", &EMOJI_LIST[1496] },
{ "🔽", &EMOJI_LIST[1497] },
{ "⏬", &EMOJI_LIST[1498] },
{ "⏸️", &EMOJI_LIST[1499] },
{ "⏹️", &EMOJI_LIST[1500] },
{ "⏺️", &EMOJI_LIST[1501] },
{ "⏏️", &EMOJI_LIST[1502] },
{ "🎦", &EMOJI_LIST[1503] },
{ "🔅", &EMOJI_LIST[1504] },
{ "🔆", &EMOJI_LIST[1505] },
{ "📶", &EMOJI_LIST[1506] },
{ "🛜", &EMOJI_LIST[1507] },
{ "📳", &EMOJI_LIST[1508] },
{ "📴", &EMOJI_LIST[1509] },
{ "♀️", &EMOJI_LIST[1510] },
{ "♂️", &EMOJI_LIST[1511] },
{ "⚧️", &EMOJI_LIST[1512] },
{ "✖️", &EMOJI_LIST[1513] },
{ "➕", &EMOJI_LIST[1514] },
{ "➖", &EMOJI_LIST[1515] },
{ "➗", &EMOJI_LIST[1516] },
{ "🟰", &EMOJI_LIST[1517] },
{ "♾️", &EMOJI_LIST[1518] },
{ "‼️", &EMOJI_LIST[1519] },
{ "⁉️", &EMOJI_LIST[1520] },
{ "❓", &EMOJI_LIST[1521] },
{ "❔", &EMOJI_LIST[1522] },
{ "❕", &EMOJI_LIST[1523] },
{ "❗", &EMOJI_LIST[1524] },
{ "〰️", &EMOJI_LIST[1525] },
{ "💱", &EMOJI_LIST[1526] },
{ "💲", &EMOJI_LIST[1527] },
{ "⚕️", &EMOJI_LIST[1528] },
{ "♻️", &EMOJI_LIST[1529] },
{ "⚜️", &EMOJI_LIST[1530] },
{ "🔱", &EMOJI_LIST[1531] },
{ "📛", &EMOJI_LIST[1532] },
{ "🔰", &EMOJI_LIST[1533] },
{ "⭕", &EMOJI_LIST[1534] },
{ "✅", &EMOJI_LIST[1535] },
{ "☑️", &EMOJI_LIST[1536] },
{ "✔️", &EMOJI_LIST[1537] },
{ "❌", &EMOJI_LIST[1538] },
{ "❎", &EMOJI_LIST[1539] },
{ "➰", &EMOJI_LIST[1540] },
{ "➿", &EMOJI_LIST[1541] },
{ "〽️", &EMOJI_LIST[1542] },
{ "✳️", &EMOJI_LIST[1543] },
{ "✴️", &EMOJI_LIST[1544] },
{ "❇️", &EMOJI_LIST[1545] },
{ "©️", &EMOJI_LIST[1546] },
{ "®️", &EMOJI_LIST[1547] },
{ "™️", &EMOJI_LIST[1548] },
{ "🫟", &EMOJI_LIST[1549] },
{ "#️⃣", &EMOJI_LIST[1550] },
{ "*️⃣", &EMOJI_LIST[1551] },
{ "0️⃣", &EMOJI_LIST[1552] },
{ "1️⃣", &EMOJI_LIST[1553] },
{ "2️⃣", &EMOJI_LIST[1554] },
{ "3️⃣", &EMOJI_LIST[1555] },
{ "4️⃣", &EMOJI_LIST[1556] },
{ "5️⃣", &EMOJI_LIST[1557] },
{ "6️⃣", &EMOJI_LIST[1558] },
{ "7️⃣", &EMOJI_LIST[1559] },
{ "8️⃣", &EMOJI_LIST[1560] },
{ "9️⃣", &EMOJI_LIST[1561] },
{ "🔟", &EMOJI_LIST[1562] },
{ "🔠", &EMOJI_LIST[1563] },
{ "🔡", &EMOJI_LIST[1564] },
{ "🔢", &EMOJI_LIST[1565] },
{ "🔣", &EMOJI_LIST[1566] },
{ "🔤", &EMOJI_LIST[1567] },
{ "🅰️", &EMOJI_LIST[1568] },
{ "🆎", &EMOJI_LIST[1569] },
{ "🅱️", &EMOJI_LIST[1570] },
{ "🆑", &EMOJI_LIST[1571] },
{ "🆒", &EMOJI_LIST[1572] },
{ "🆓", &EMOJI_LIST[1573] },
{ "ℹ️", &EMOJI_LIST[1574] },
{ "🆔", &EMOJI_LIST[1575] },
{ "Ⓜ️", &EMOJI_LIST[1576] },
{ "🆕", &EMOJI_LIST[1577] },
{ "🆖", &EMOJI_LIST[1578] },
{ "🅾️", &EMOJI_LIST[1579] },
{ "🆗", &EMOJI_LIST[1580] },
{ "🅿️", &EMOJI_LIST[1581] },
{ "🆘", &EMOJI_LIST[1582] },
{ "🆙", &EMOJI_LIST[1583] },
{ "🆚", &EMOJI_LIST[1584] },
{ "🈁", &EMOJI_LIST[1585] },
{ "🈂️", &EMOJI_LIST[1586] },
{ "🈷️", &EMOJI_LIST[1587] },
{ "🈶", &EMOJI_LIST[1588] },
{ "🈯", &EMOJI_LIST[1589] },
{ "🉐", &EMOJI_LIST[1590] },
{ "🈹", &EMOJI_LIST[1591] },
{ "🈚", &EMOJI_LIST[1592] },
{ "🈲", &EMOJI_LIST[1593] },
{ "🉑", &EMOJI_LIST[1594] },
{ "🈸", &EMOJI_LIST[1595] },
{ "🈴", &EMOJI_LIST[1596] },
{ "🈳", &EMOJI_LIST[1597] },
{ "㊗️", &EMOJI_LIST[1598] },
{ "㊙️", &EMOJI_LIST[1599] },
{ "🈺", &EMOJI_LIST[1600] },
{ "🈵", &EMOJI_LIST[1601] },
{ "🔴", &EMOJI_LIST[1602] },
{ "🟠", &EMOJI_LIST[1603] },
{ "🟡", &EMOJI_LIST[1604] },
{ "🟢", &EMOJI_LIST[1605] },
{ "🔵", &EMOJI_LIST[1606] },
{ "🟣", &EMOJI_LIST[1607] },
{ "🟤", &EMOJI_LIST[1608] },
{ "⚫", &EMOJI_LIST[1609] },
{ "⚪", &EMOJI_LIST[1610] },
{ "🟥", &EMOJI_LIST[1611] },
{ "🟧", &EMOJI_LIST[1612] },
{ "🟨", &EMOJI_LIST[1613] },
{ "🟩", &EMOJI_LIST[1614] },
{ "🟦", &EMOJI_LIST[1615] },
{ "🟪", &EMOJI_LIST[1616] },
{ "🟫", &EMOJI_LIST[1617] },
{ "⬛", &EMOJI_LIST[1618] },
{ "⬜", &EMOJI_LIST[1619] },
{ "◼️", &EMOJI_LIST[1620] },
{ "◻️", &EMOJI_LIST[1621] },
{ "◾", &EMOJI_LIST[1622] },
{ "◽", &EMOJI_LIST[1623] },
{ "▪️", &EMOJI_LIST[1624] },
{ "▫️", &EMOJI_LIST[1625] },
{ "🔶", &EMOJI_LIST[1626] },
{ "🔷", &EMOJI_LIST[1627] },
{ "🔸", &EMOJI_LIST[1628] },
{ "🔹", &EMOJI_LIST[1629] },
{ "🔺", &EMOJI_LIST[1630] },
{ "🔻", &EMOJI_LIST[1631] },
{ "💠", &EMOJI_LIST[1632] },
{ "🔘", &EMOJI_LIST[1633] },
{ "🔳", &EMOJI_LIST[1634] },
{ "🔲", &EMOJI_LIST[1635] },
{ "🏁", &EMOJI_LIST[1636] },
{ "🚩", &EMOJI_LIST[1637] },
{ "🎌", &EMOJI_LIST[1638] },
{ "🏴", &EMOJI_LIST[1639] },
{ "🏳️", &EMOJI_LIST[1640] },
{ "🏳️‍🌈", &EMOJI_LIST[1641] },
{ "🏳️‍⚧️", &EMOJI_LIST[1642] },
{ "🏴‍☠️", &EMOJI_LIST[1643] },
{ "🇦🇨", &EMOJI_LIST[1644] },
{ "🇦🇩", &EMOJI_LIST[1645] },
{ "🇦🇪", &EMOJI_LIST[1646] },
{ "🇦🇫", &EMOJI_LIST[1647] },
{ "🇦🇬", &EMOJI_LIST[1648] },
{ "🇦🇮", &EMOJI_LIST[1649] },
{ "🇦🇱", &EMOJI_LIST[1650] },
{ "🇦🇲", &EMOJI_LIST[1651] },
{ "🇦🇴", &EMOJI_LIST[1652] },
{ "🇦🇶", &EMOJI_LIST[1653] },
{ "🇦🇷", &EMOJI_LIST[1654] },
{ "🇦🇸", &EMOJI_LIST[1655] },
{ "🇦🇹", &EMOJI_LIST[1656] },
{ "🇦🇺", &EMOJI_LIST[1657] },
{ "🇦🇼", &EMOJI_LIST[1658] },
{ "🇦🇽", &EMOJI_LIST[1659] },
{ "🇦🇿", &EMOJI_LIST[1660] },
{ "🇧🇦", &EMOJI_LIST[1661] },
{ "🇧🇧", &EMOJI_LIST[1662] },
{ "🇧🇩", &EMOJI_LIST[1663] },
{ "🇧🇪", &EMOJI_LIST[1664] },
{ "🇧🇫", &EMOJI_LIST[1665] },
{ "🇧🇬", &EMOJI_LIST[1666] },
{ "🇧🇭", &EMOJI_LIST[1667] },
{ "🇧🇮", &EMOJI_LIST[1668] },
{ "🇧🇯", &EMOJI_LIST[1669] },
{ "🇧🇱", &EMOJI_LIST[1670] },
{ "🇧🇲", &EMOJI_LIST[1671] },
{ "🇧🇳", &EMOJI_LIST[1672] },
{ "🇧🇴", &EMOJI_LIST[1673] },
{ "🇧🇶", &EMOJI_LIST[1674] },
{ "🇧🇷", &EMOJI_LIST[1675] },
{ "🇧🇸", &EMOJI_LIST[1676] },
{ "🇧🇹", &EMOJI_LIST[1677] },
{ "🇧🇻", &EMOJI_LIST[1678] },
{ "🇧🇼", &EMOJI_LIST[1679] },
{ "🇧🇾", &EMOJI_LIST[1680] },
{ "🇧🇿", &EMOJI_LIST[1681] },
{ "🇨🇦", &EMOJI_LIST[1682] },
{ "🇨🇨", &EMOJI_LIST[1683] },
{ "🇨🇩", &EMOJI_LIST[1684] },
{ "🇨🇫", &EMOJI_LIST[1685] },
{ "🇨🇬", &EMOJI_LIST[1686] },
{ "🇨🇭", &EMOJI_LIST[1687] },
{ "🇨🇮", &EMOJI_LIST[1688] },
{ "🇨🇰", &EMOJI_LIST[1689] },
{ "🇨🇱", &EMOJI_LIST[1690] },
{ "🇨🇲", &EMOJI_LIST[1691] },
{ "🇨🇳", &EMOJI_LIST[1692] },
{ "🇨🇴", &EMOJI_LIST[1693] },
{ "🇨🇵", &EMOJI_LIST[1694] },
{ "🇨🇶", &EMOJI_LIST[1695] },
{ "🇨🇷", &EMOJI_LIST[1696] },
{ "🇨🇺", &EMOJI_LIST[1697] },
{ "🇨🇻", &EMOJI_LIST[1698] },
{ "🇨🇼", &EMOJI_LIST[1699] },
{ "🇨🇽", &EMOJI_LIST[1700] },
{ "🇨🇾", &EMOJI_LIST[1701] },
{ "🇨🇿", &EMOJI_LIST[1702] },
{ "🇩🇪", &EMOJI_LIST[1703] },
{ "🇩🇬", &EMOJI_LIST[1704] },
{ "🇩🇯", &EMOJI_LIST[1705] },
{ "🇩🇰", &EMOJI_LIST[1706] },
{ "🇩🇲", &EMOJI_LIST[1707] },
{ "🇩🇴", &EMOJI_LIST[1708] },
{ "🇩🇿", &EMOJI_LIST[1709] },
{ "🇪🇦", &EMOJI_LIST[1710] },
{ "🇪🇨", &EMOJI_LIST[1711] },
{ "🇪🇪", &EMOJI_LIST[1712] },
{ "🇪🇬", &EMOJI_LIST[1713] },
{ "🇪🇭", &EMOJI_LIST[1714] },
{ "🇪🇷", &EMOJI_LIST[1715] },
{ "🇪🇸", &EMOJI_LIST[1716] },
{ "🇪🇹", &EMOJI_LIST[1717] },
{ "🇪🇺", &EMOJI_LIST[1718] },
{ "🇫🇮", &EMOJI_LIST[1719] },
{ "🇫🇯", &EMOJI_LIST[1720] },
{ "🇫🇰", &EMOJI_LIST[1721] },
{ "🇫🇲", &EMOJI_LIST[1722] },
{ "🇫🇴", &EMOJI_LIST[1723] },
{ "🇫🇷", &EMOJI_LIST[1724] },
{ "🇬🇦", &EMOJI_LIST[1725] },
{ "🇬🇧", &EMOJI_LIST[1726] },
{ "🇬🇩", &EMOJI_LIST[1727] },
{ "🇬🇪", &EMOJI_LIST[1728] },
{ "🇬🇫", &EMOJI_LIST[1729] },
{ "🇬🇬", &EMOJI_LIST[1730] },
{ "🇬🇭", &EMOJI_LIST[1731] },
{ "🇬🇮", &EMOJI_LIST[1732] },
{ "🇬🇱", &EMOJI_LIST[1733] },
{ "🇬🇲", &EMOJI_LIST[1734] },
{ "🇬🇳", &EMOJI_LIST[1735] },
{ "🇬🇵", &EMOJI_LIST[1736] },
{ "🇬🇶", &EMOJI_LIST[1737] },
{ "🇬🇷", &EMOJI_LIST[1738] },
{ "🇬🇸", &EMOJI_LIST[1739] },
{ "🇬🇹", &EMOJI_LIST[1740] },
{ "🇬🇺", &EMOJI_LIST[1741] },
{ "🇬🇼", &EMOJI_LIST[1742] },
{ "🇬🇾", &EMOJI_LIST[1743] },
{ "🇭🇰", &EMOJI_LIST[1744] },
{ "🇭🇲", &EMOJI_LIST[1745] },
{ "🇭🇳", &EMOJI_LIST[1746] },
{ "🇭🇷", &EMOJI_LIST[1747] },
{ "🇭🇹", &EMOJI_LIST[1748] },
{ "🇭🇺", &EMOJI_LIST[1749] },
{ "🇮🇨", &EMOJI_LIST[1750] },
{ "🇮🇩", &EMOJI_LIST[1751] },
{ "🇮🇪", &EMOJI_LIST[1752] },
{ "🇮🇱", &EMOJI_LIST[1753] },
{ "🇮🇲", &EMOJI_LIST[1754] },
{ "🇮🇳", &EMOJI_LIST[1755] },
{ "🇮🇴", &EMOJI_LIST[1756] },
{ "🇮🇶", &EMOJI_LIST[1757] },
{ "🇮🇷", &EMOJI_LIST[1758] },
{ "🇮🇸", &EMOJI_LIST[1759] },
{ "🇮🇹", &EMOJI_LIST[1760] },
{ "🇯🇪", &EMOJI_LIST[1761] },
{ "🇯🇲", &EMOJI_LIST[1762] },
{ "🇯🇴", &EMOJI_LIST[1763] },
{ "🇯🇵", &EMOJI_LIST[1764] },
{ "🇰🇪", &EMOJI_LIST[1765] },
{ "🇰🇬", &EMOJI_LIST[1766] },
{ "🇰🇭", &EMOJI_LIST[1767] },
{ "🇰🇮", &EMOJI_LIST[1768] },
{ "🇰🇲", &EMOJI_LIST[1769] },
{ "🇰🇳", &EMOJI_LIST[1770] },
{ "🇰🇵", &EMOJI_LIST[1771] },
{ "🇰🇷", &EMOJI_LIST[1772] },
{ "🇰🇼", &EMOJI_LIST[1773] },
{ "🇰🇾", &EMOJI_LIST[1774] },
{ "🇰🇿", &EMOJI_LIST[1775] },
{ "🇱🇦", &EMOJI_LIST[1776] },
{ "🇱🇧", &EMOJI_LIST[1777] },
{ "🇱🇨", &EMOJI_LIST[1778] },
{ "🇱🇮", &EMOJI_LIST[1779] },
{ "🇱🇰", &EMOJI_LIST[1780] },
{ "🇱🇷", &EMOJI_LIST[1781] },
{ "🇱🇸", &EMOJI_LIST[1782] },
{ "🇱🇹", &EMOJI_LIST[1783] },
{ "🇱🇺", &EMOJI_LIST[1784] },
{ "🇱🇻", &EMOJI_LIST[1785] },
{ "🇱🇾", &EMOJI_LIST[1786] },
{ "🇲🇦", &EMOJI_LIST[1787] },
{ "🇲🇨", &EMOJI_LIST[1788] },
{ "🇲🇩", &EMOJI_LIST[1789] },
{ "🇲🇪", &EMOJI_LIST[1790] },
{ "🇲🇫", &EMOJI_LIST[1791] },
{ "🇲🇬", &EMOJI_LIST[1792] },
{ "🇲🇭", &EMOJI_LIST[1793] },
{ "🇲🇰", &EMOJI_LIST[1794] },
{ "🇲🇱", &EMOJI_LIST[1795] },
{ "🇲🇲", &EMOJI_LIST[1796] },
{ "🇲🇳", &EMOJI_LIST[1797] },
{ "🇲🇴", &EMOJI_LIST[1798] },
{ "🇲🇵", &EMOJI_LIST[1799] },
{ "🇲🇶", &EMOJI_LIST[1800] },
{ "🇲🇷", &EMOJI_LIST[1801] },
{ "🇲🇸", &EMOJI_LIST[1802] },
{ "🇲🇹", &EMOJI_LIST[1803] },
{ "🇲🇺", &EMOJI_LIST[1804] },
{ "🇲🇻", &EMOJI_LIST[1805] },
{ "🇲🇼", &EMOJI_LIST[1806] },
{ "🇲🇽", &EMOJI_LIST[1807] },
{ "🇲🇾", &EMOJI_LIST[1808] },
{ "🇲🇿", &EMOJI_LIST[1809] },
{ "🇳🇦", &EMOJI_LIST[1810] },
{ "🇳🇨", &EMOJI_LIST[1811] },
{ "🇳🇪", &EMOJI_LIST[1812] },
{ "🇳🇫", &EMOJI_LIST[1813] },
{ "🇳🇬", &EMOJI_LIST[1814] },
{ "🇳🇮", &EMOJI_LIST[1815] },
{ "🇳🇱", &EMOJI_LIST[1816] },
{ "🇳🇴", &EMOJI_LIST[1817] },
{ "🇳🇵", &EMOJI_LIST[1818] },
{ "🇳🇷", &EMOJI_LIST[1819] },
{ "🇳🇺", &EMOJI_LIST[1820] },
{ "🇳🇿", &EMOJI_LIST[1821] },
{ "🇴🇲", &EMOJI_LIST[1822] },
{ "🇵🇦", &EMOJI_LIST[1823] },
{ "🇵🇪", &EMOJI_LIST[1824] },
{ "🇵🇫", &EMOJI_LIST[1825] },
{ "🇵🇬", &EMOJI_LIST[1826] },
{ "🇵🇭", &EMOJI_LIST[1827] },
{ "🇵🇰", &EMOJI_LIST[1828] },
{ "🇵🇱", &EMOJI_LIST[1829] },
{ "🇵🇲", &EMOJI_LIST[1830] },
{ "🇵🇳", &EMOJI_LIST[1831] },
{ "🇵🇷", &EMOJI_LIST[1832] },
{ "🇵🇸", &EMOJI_LIST[1833] },
{ "🇵🇹", &EMOJI_LIST[1834] },
{ "🇵🇼", &EMOJI_LIST[1835] },
{ "🇵🇾", &EMOJI_LIST[1836] },
{ "🇶🇦", &EMOJI_LIST[1837] },
{ "🇷🇪", &EMOJI_LIST[1838] },
{ "🇷🇴", &EMOJI_LIST[1839] },
{ "🇷🇸", &EMOJI_LIST[1840] },
{ "🇷🇺", &EMOJI_LIST[1841] },
{ "🇷🇼", &EMOJI_LIST[1842] },
{ "🇸🇦", &EMOJI_LIST[1843] },
{ "🇸🇧", &EMOJI_LIST[1844] },
{ "🇸🇨", &EMOJI_LIST[1845] },
{ "🇸🇩", &EMOJI_LIST[1846] },
{ "🇸🇪", &EMOJI_LIST[1847] },
{ "🇸🇬", &EMOJI_LIST[1848] },
{ "🇸🇭", &EMOJI_LIST[1849] },
{ "🇸🇮", &EMOJI_LIST[1850] },
{ "🇸🇯", &EMOJI_LIST[1851] },
{ "🇸🇰", &EMOJI_LIST[1852] },
{ "🇸🇱", &EMOJI_LIST[1853] },
{ "🇸🇲", &EMOJI_LIST[1854] },
{ "🇸🇳", &EMOJI_LIST[1855] },
{ "🇸🇴", &EMOJI_LIST[1856] },
{ "🇸🇷", &EMOJI_LIST[1857] },
{ "🇸🇸", &EMOJI_LIST[1858] },
{ "🇸🇹", &EMOJI_LIST[1859] },
{ "🇸🇻", &EMOJI_LIST[1860] },
{ "🇸🇽", &EMOJI_LIST[1861] },
{ "🇸🇾", &EMOJI_LIST[1862] },
{ "🇸🇿", &EMOJI_LIST[1863] },
{ "🇹🇦", &EMOJI_LIST[1864] },
{ "🇹🇨", &EMOJI_LIST[1865] },
{ "🇹🇩", &EMOJI_LIST[1866] },
{ "🇹🇫", &EMOJI_LIST[1867] },
{ "🇹🇬", &EMOJI_LIST[1868] },
{ "🇹🇭", &EMOJI_LIST[1869] },
{ "🇹🇯", &EMOJI_LIST[1870] },
{ "🇹🇰", &EMOJI_LIST[1871] },
{ "🇹🇱", &EMOJI_LIST[1872] },
{ "🇹🇲", &EMOJI_LIST[1873] },
{ "🇹🇳", &EMOJI_LIST[1874] },
{ "🇹🇴", &EMOJI_LIST[1875] },
{ "🇹🇷", &EMOJI_LIST[1876] },
{ "🇹🇹", &EMOJI_LIST[1877] },
{ "🇹🇻", &EMOJI_LIST[1878] },
{ "🇹🇼", &EMOJI_LIST[1879] },
{ "🇹🇿", &EMOJI_LIST[1880] },
{ "🇺🇦", &EMOJI_LIST[1881] },
{ "🇺🇬", &EMOJI_LIST[1882] },
{ "🇺🇲", &EMOJI_LIST[1883] },
{ "🇺🇳", &EMOJI_LIST[1884] },
{ "🇺🇸", &EMOJI_LIST[1885] },
{ "🇺🇾", &EMOJI_LIST[1886] },
{ "🇺🇿", &EMOJI_LIST[1887] },
{ "🇻🇦", &EMOJI_LIST[1888] },
{ "🇻🇨", &EMOJI_LIST[1889] },
{ "🇻🇪", &EMOJI_LIST[1890] },
{ "🇻🇬", &EMOJI_LIST[1891] },
{ "🇻🇮", &EMOJI_LIST[1892] },
{ "🇻🇳", &EMOJI_LIST[1893] },
{ "🇻🇺", &EMOJI_LIST[1894] },
{ "🇼🇫", &EMOJI_LIST[1895] },
{ "🇼🇸", &EMOJI_LIST[1896] },
{ "🇽🇰", &EMOJI_LIST[1897] },
{ "🇾🇪", &EMOJI_LIST[1898] },
{ "🇾🇹", &EMOJI_LIST[1899] },
{ "🇿🇦", &EMOJI_LIST[1900] },
{ "🇿🇲", &EMOJI_LIST[1901] },
{ "🇿🇼", &EMOJI_LIST[1902] },
{ "🏴󠁧󠁢󠁥󠁮󠁧󠁿", &EMOJI_LIST[1903] },
{ "🏴󠁧󠁢󠁳󠁣󠁴󠁿", &EMOJI_LIST[1904] },
{ "🏴󠁧󠁢󠁷󠁬󠁳󠁿", &EMOJI_LIST[1905] }
};
 const std::unordered_map<std::string_view, const EmojiData*>& StaticEmojiDatabase::mapping() { return MAPPING; }

const std::array<std::string_view, 9>& StaticEmojiDatabase::groups() { return GROUPS; }