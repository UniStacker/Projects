import unittest
import os
import shutil
from baby_general_learner import BabyGeneralLearner

class TestBabyGeneralLearner(unittest.TestCase):

    def setUp(self):
        self.store_dir = "test_bgl_store"
        if os.path.exists(self.store_dir):
            shutil.rmtree(self.store_dir)
        os.makedirs(self.store_dir)

    def tearDown(self):
        if os.path.exists(self.store_dir):
            shutil.rmtree(self.store_dir)

    def test_text_classifier_scaffold(self):
        bgl = BabyGeneralLearner(store_dir=self.store_dir, scaffold="text_classifier")
        self.assertIsNotNone(bgl)
        
        train_data = [
            ("I love this movie, it's fantastic!", "positive"),
            ("What a waste of time, terrible plot.", "negative"),
        ]
        
        for text, label in train_data:
            doc_id = bgl.add_docs([text])[0]
            bgl.train(doc_id, label)
            
        self.assertEqual(len(bgl.labels["positive"]), 1)
        self.assertEqual(len(bgl.labels["negative"]), 1)
        
        pred = bgl.predict("A truly heartwarming and beautiful film.")
        self.assertEqual(pred[0][0], "positive")

    def test_qa_scaffold(self):
        bgl = BabyGeneralLearner(store_dir=self.store_dir, scaffold="qa")
        self.assertIsNotNone(bgl)
        
        bgl.add_qa("What is the capital of France?", "Paris is the capital of France.")
        
        self.assertEqual(len(bgl.questions), 1)
        self.assertEqual(len(bgl.answers), 1)
        
        answers = bgl.answer("capital of france")
        self.assertEqual(len(answers), 1)
        self.assertEqual(answers[0]['answer'], "Paris is the capital of France.")

if __name__ == '__main__':
    unittest.main()
