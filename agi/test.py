from baby_general_learner import BabyGeneralLearner

docs = [
    # Science
    "Atoms consist of protons, neutrons, and electrons. Protons determine the atomic number.",
    "DNA encodes genetic information using four bases: adenine, cytosine, guanine, thymine.",
    "Black holes form when massive stars collapse; their gravity is so strong even light cannot escape.",
    "The speed of light in vacuum is about 3×10^8 meters per second.",
    "Photosynthesis converts carbon dioxide and water into glucose and oxygen using sunlight.",
    "Evolution acts through natural selection, where advantageous traits spread across generations.",
    "Entropy measures disorder in a system and tends to increase.",
    
    # Cooking
    "Yeast ferments sugars to produce carbon dioxide, which makes bread rise.",
    "Umami flavor comes from glutamates found in ingredients like soy sauce and mushrooms.",
    "Marinades tenderize meat by breaking down proteins with acids or enzymes.",
    "Baking is chemistry: precise ratios of flour, fat, sugar, and leavening create textures.",
    "Caramelization is the breakdown of sugars under heat, producing nutty, sweet flavors.",
    
    # Programming
    "Algorithms are step-by-step procedures for solving problems.",
    "Big O notation measures how an algorithm scales with input size.",
    "Recursion is a function calling itself with smaller inputs.",
    "Hash functions map data to fixed-size values; good ones avoid collisions.",
    "Deadlocks occur in concurrent systems when processes wait on each other forever.",
    "Garbage collection frees unused memory in high-level programming languages.",
    "Unit tests verify small pieces of code behave as expected.",
    
    # History
    "The Roman Empire fell in 476 CE when the last emperor was deposed.",
    "The French Revolution in 1789 overthrew the monarchy and promoted liberty, equality, fraternity.",
    "World War I began in 1914 after the assassination of Archduke Franz Ferdinand.",
    "The Industrial Revolution introduced steam engines, factories, and mass production.",
    "The Cold War was a geopolitical standoff between the USA and USSR after WWII.",
    
    # Culture
    "Haiku is a Japanese poetic form with a 5-7-5 syllable structure.",
    "Chess originated in India and evolved into modern rules by the 15th century.",
    "The periodic table arranges elements by atomic number and recurring chemical properties.",
    "Bitcoin is a decentralized digital currency based on blockchain.",
    "Memes are cultural units of information that spread virally."
]

tags = [
    # Science
    ["science", "physics", "atoms"],
    ["science", "biology", "genetics"],
    ["science", "astronomy", "black_holes"],
    ["science", "physics", "fundamental_constants"],
    ["science", "biology", "photosynthesis"],
    ["science", "biology", "evolution"],
    ["science", "physics", "thermodynamics"],
    
    # Cooking
    ["cooking", "baking", "fermentation"],
    ["cooking", "flavor", "umami"],
    ["cooking", "meat", "marinade"],
    ["cooking", "baking", "chemistry"],
    ["cooking", "sugars", "caramelization"],
    
    # Programming
    ["programming", "algorithms", "basics"],
    ["programming", "algorithms", "complexity"],
    ["programming", "algorithms", "recursion"],
    ["programming", "data_structures", "hashing"],
    ["programming", "concurrency", "deadlocks"],
    ["programming", "memory", "garbage_collection"],
    ["programming", "testing", "unit_tests"],
    
    # History
    ["history", "rome", "fall_of_empire"],
    ["history", "france", "revolution"],
    ["history", "ww1", "20th_century"],
    ["history", "industrial", "revolution"],
    ["history", "cold_war", "20th_century"],
    
    # Culture
    ["culture", "literature", "poetry"],
    ["culture", "games", "chess"],
    ["science", "chemistry", "periodic_table"],
    ["culture", "technology", "cryptocurrency"],
    ["culture", "memes", "internet"]
]

stress_queries = [
    # 1. Recall under disguise
    "What subatomic particle decides the atomic number of an element?",
    "Why do onions turn sweet when cooked slowly instead of burnt when cooked fast?",
    "Which historical event started after the murder of Archduke Ferdinand?",
    "What does Big O notation tell you about code efficiency?",
    "Which poetic form from Japan has a strict syllable rule of 5-7-5?",

    # 2. Association challenges
    "What’s the link between yeast and bread rising?",
    "Why do soy sauce and mushrooms taste similar to the human tongue?",
    "How is entropy connected to the arrow of time?",
    "What connects steam engines to mass production?",
    "What’s the similarity between deadlocks in computing and cars at a four-way stop?",

    # 3. Cross-domain analogies
    "How is recursion in programming similar to Russian nesting dolls?",
    "Explain evolution in biology using version control systems as an analogy.",
    "What makes the French Revolution similar to the open-source movement?",
    "Compare garbage collection in programming to doing laundry at home.",
    "How is entropy in physics similar to your messy bedroom?",

    # 4. Multi-hop reasoning
    "If the Industrial Revolution introduced steam engines, and factories rely on mass production, what did this do to human populations?",
    "Why does photosynthesis matter for human survival even though it happens in plants?",
    "If black holes trap light, what does that say about their escape velocity?",
    "What role did the Cold War play in the development of digital technology like the internet?",
    "What’s the connection between the periodic table and chemical reactions?",

    # 5. Trick/generalization
    "Why can’t you escape a black hole even if you go faster than sound?",
    "What makes bread fluffy but also beer fizzy?",
    "Why are unit tests like seatbelts in a car?",
    "What’s the historical pattern connecting revolutions in France, industry, and computers?",
    "Why is Bitcoin called ‘decentralized’ compared to traditional money?",
]

bgl = BabyGeneralLearner(store_dir="test-store")

if bgl.N_docs == 0:
    bgl.add_docs(docs, tags)

for query in stress_queries:
    print("="*100)
    print(f"Query: {query}")
    print("Docs:")
    for d in bgl.explain(query, topk=1).get("docs"):
        print(f"  * [{d.get("score"):.2f}] {d.get("text")} | {d.get("tags")}")
