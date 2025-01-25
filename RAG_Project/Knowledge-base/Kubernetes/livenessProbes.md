spec:
      containers:
      - name: second-node
        image: jbite9057/kub-first-app
        livenessProbe:
          httpGet:
            path: /
            port: 8080
          periodSenconds: 10
          initialDelaySeconds: 5

