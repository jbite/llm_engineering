
we can add environment variable in deployment.yaml

 spec:
      containers:
        - name: story
          image: jbite9057/kub-data-demo:2
          env:
            - name: STORY_FOLDER
              value: 'story'

when we want to use environment as a file to apply, we create a environment.yaml

apiVersion: v1
kind: ConfigMap
metadata:
  name: data-store-env
data:
  folder: 'story'
  # key: value..


and put this in deployment.yaml to replace under spec.containers.env as follow
spec:
      containers:
        - name: story
          image: jbite9057/kub-data-demo:2
          env:
            - name: STORY_FOLDER
              valueFrom:
                configMapKeyRef:
                  name: data-store-env
                  key: folder

